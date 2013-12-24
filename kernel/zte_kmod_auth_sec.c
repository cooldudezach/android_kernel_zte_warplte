/*
 *  Copyright (C) 2013 Jia.jia ZTE Corp.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * Date         Author           Comment
 * -----------  --------------   --------------------------------------
 * 2013-06-09   Jia              created by ZTE_BOO_JIA_20130609 jia.jia
 * --------------------------------------------------------------------
 */

#include <linux/export.h>
#include <linux/moduleloader.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/elf.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/capability.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/sysdev.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/crypto.h>
#include <crypto/hash.h>
#include <crypto/sha.h>
#include <linux/version.h>

#include <mach/scm.h>

/*
 * Macro Definition
 */
#define AUTH_SEC_VERSION     "1.0"
#define AUTH_SEC_CLASS_NAME  "zte_kmod_auth_sec"

#define AUTH_SEC_PHNUM  (3)

#define MI_BOOT_IMG_HDR_SIZE  (40)

#define PAS_INIT_IMAGE_CMD	(1)

/*
 * Type Definition
 */
// Refer to arch/arm/mach-msm/scm-pas.h
enum pas_id {
	PAS_MODEM,
	PAS_Q6,
	PAS_DSPS,
	PAS_TZAPPS,
	PAS_MODEM_SW,
	PAS_MODEM_FW,
	PAS_WCNSS,
	PAS_SECAPP,
	PAS_GSS,
	PAS_VIDC,
	PAS_KMOD_AUTH_SEC,
};

typedef struct {
	uint8_t  *elfent;    // ELF file
	uint32_t elfent_len; // length of ELF file

	Elf32_Ehdr ehdr;  // header of ELF
	Elf32_Phdr *phdr; // array of program headers of ELF

	uint32_t certst; // signature & cert chain status
} auth_sec_file_t;

/*
 * Global Variables Definition
 */
static auth_sec_file_t auth_sec_file;
static struct sys_device auth_sec_sysdev;
static struct crypto_shash *auth_sec_shash;

DEFINE_MUTEX(auth_sec_lock);

/*
 * Function declaration
 */
static int32_t auth_sec_chk_elf_hdr(const auth_sec_file_t *as);
static int32_t auth_sec_cert_sig_certchain(const auth_sec_file_t *as);
static int32_t auth_sec_gen_hash(struct crypto_shash *shash, const uint8_t *data, uint32_t len, uint8_t *hash);
static int32_t auth_sec_cert_hash(const auth_sec_file_t *as, struct crypto_shash *shash);
static int32_t auth_sec_populate_elf_phdr_tbl(const auth_sec_file_t *as, Elf32_Phdr *tbl);

/*
 * Function Definition
 */
/*
 * Check ELF header
 */
static int32_t auth_sec_chk_elf_hdr(const auth_sec_file_t *as)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)as->elfent;

	if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0
		|| ehdr->e_ident[EI_CLASS] != ELFCLASS32
		|| ehdr->e_ident[EI_VERSION] != EV_CURRENT
		|| ehdr->e_ehsize != sizeof(Elf32_Ehdr)
		|| ehdr->e_phentsize != sizeof(Elf32_Phdr)
		|| ehdr->e_phnum != AUTH_SEC_PHNUM) {
		return -EINVAL;
	}

	return 0;
}

/*
 * Certify signature & cert chain
 */
static int32_t auth_sec_cert_sig_certchain(const auth_sec_file_t *as)
{
	struct pas_init_image_req {
		uint32_t proc;
		uint32_t image_addr;
	} request;

	void *mdata_buf = NULL;
	uint32_t scm_ret = 0;
	int32_t ret = 0;

	/*
	 * Make memory physically contiguous
	 */
	mdata_buf = kmemdup(as->elfent, as->elfent_len, GFP_KERNEL);
	if (mdata_buf == NULL) {
		return -ENOMEM;
	}

	request.proc = PAS_KMOD_AUTH_SEC;
	request.image_addr = virt_to_phys(mdata_buf);

	ret = scm_call(SCM_SVC_PIL, PAS_INIT_IMAGE_CMD, &request,
					sizeof(request), &scm_ret, sizeof(scm_ret));
	kfree(mdata_buf);

	if (ret != 0) {
		return ret;
	}

	return scm_ret;
}

/*
 * Generate hash
 */
static int32_t auth_sec_gen_hash(struct crypto_shash *shash, const uint8_t *data, uint32_t len, uint8_t *hash)
{
	struct shash_desc *desc = NULL;

	desc = (struct shash_desc *)kzalloc(sizeof(*desc) + crypto_shash_descsize(shash), GFP_KERNEL);
	if (!desc) {
		return -EINVAL;
	}

	desc->tfm = shash;
	desc->flags = CRYPTO_TFM_REQ_MAY_SLEEP;

	crypto_shash_init(desc);
	crypto_shash_update(desc, data, len);
	crypto_shash_final(desc, hash);

	kfree(desc);

	return 0;

}

/*
 * Certify hash segments
 */
static int32_t auth_sec_cert_hash(const auth_sec_file_t *as, struct crypto_shash *shash)
{
	uint8_t *data = NULL;
	uint32_t data_offset = 0, data_len = 0;
	uint8_t *buf = NULL;
	uint32_t buf_offset = 0;
	uint8_t hash[SHA1_DIGEST_SIZE];
	int32_t ret = 0;

	/*
	 * Generate hash for ELF head & program header table
	 */
	data_offset = 0;
	data = as->elfent + data_offset;
	data_len = as->ehdr.e_ehsize + (as->ehdr.e_phnum * as->ehdr.e_phentsize);

	memset((void *)hash, 0, SHA1_DIGEST_SIZE);

	ret = auth_sec_gen_hash(shash, data, data_len, hash);
	if (ret != 0) {
		return ret;
	}

	/*
	 * Populate hash of ELF head & program header table
	 */
	buf_offset = as->phdr[AUTH_SEC_PHNUM - 2].p_offset + MI_BOOT_IMG_HDR_SIZE;
	buf = as->elfent + buf_offset;

	/*
	 * Verify hash for ELF head & program header table
	 */
	if (0 != memcmp((const void *)buf, (const void *)hash, SHA1_DIGEST_SIZE)) {
		return -EINVAL;
	}

	/*
	 * Generate & populate & verify hash for the hash table itself
	 */
	// Do nothing here

	/*
	 * Generate hash for code segment
	 */
	data_offset = as->phdr[AUTH_SEC_PHNUM - 1].p_offset;
	data = as->elfent + data_offset;
	data_len = as->phdr[AUTH_SEC_PHNUM - 1].p_filesz;

	memset((void *)hash, 0, SHA1_DIGEST_SIZE);

	ret = auth_sec_gen_hash(shash, data, data_len, hash);
	if (ret != 0) {
		return ret;
	}

	/*
	 * Populate hash of code segment
	 */
	buf_offset += SHA1_DIGEST_SIZE * 2;
	buf = as->elfent + buf_offset;

	/*
	 * Verify hash for code segment
	 */
	if (0 != memcmp((const void *)buf, (const void *)hash, SHA1_DIGEST_SIZE)) {
		return -EINVAL;
	}

	return 0;
}

/*
 * Populate ELF program header table
 */
static int32_t auth_sec_populate_elf_phdr_tbl(const auth_sec_file_t *as, Elf32_Phdr *tbl)
{
	memcpy((void *)tbl, (const void *)(as->elfent + as->ehdr.e_phoff), as->ehdr.e_phentsize * as->ehdr.e_phnum);

	return 0;
}

/*
 * Verify auth-sec Kmod
 */
int32_t verify_kmod_auth_sec(void __user *umod, unsigned long len)
{
	int32_t ret = 0;

	pr_debug("%s: e\n", __func__);

	/*
	 * Sanity check
	 */
	if (len < sizeof(Elf32_Ehdr)) {
		pr_err("%s: invalid values!\n", __func__);
		return -EINVAL;
	}

	mutex_lock(&auth_sec_lock);

	/*
	 * Populate ELF
	 */
	auth_sec_file.elfent_len = (uint32_t)len;
	auth_sec_file.elfent = (uint8_t *)vmalloc(auth_sec_file.elfent_len);
	if (auth_sec_file.elfent == NULL) {
		pr_err("%s: no enough memory!\n", __func__);
		mutex_unlock(&auth_sec_lock);
		return -ENOMEM;
	}

	if (copy_from_user((void *)auth_sec_file.elfent, (const void __user *)umod, (unsigned long)auth_sec_file.elfent_len) != 0) {
		pr_err("%s: failed to copy data from user!\n", __func__);
		ret = -EFAULT;
		goto verify_kmod_auth_sec_done;
	}

	/*
	 * Check ELF header
	 */
	ret = auth_sec_chk_elf_hdr(&auth_sec_file);
	if (ret != 0) {
		pr_err("%s: invalid ELF!\n", __func__);
		goto verify_kmod_auth_sec_done;
	}

	/*
	 * Populate ELF header
	 */
	memcpy((void *)&auth_sec_file.ehdr, (const void *)auth_sec_file.elfent, sizeof(Elf32_Ehdr));

	/*
	 * Populate ELF program header table
	 */
	auth_sec_file.phdr = (Elf32_Phdr *)vmalloc(auth_sec_file.ehdr.e_phentsize * auth_sec_file.ehdr.e_phnum);
	if (auth_sec_file.phdr == NULL) {
		pr_err("%s: no enough memory!\n", __func__);
		ret = -ENOMEM;
		goto verify_kmod_auth_sec_done;
	}

	ret = auth_sec_populate_elf_phdr_tbl(&auth_sec_file, auth_sec_file.phdr);
	if (ret != 0) {
		pr_err("%s: failed to populate ELF program header table!\n", __func__);
		goto verify_kmod_auth_sec_done;
	}

	/*
	 * Certify signature & cert chain
	 */
	ret = auth_sec_cert_sig_certchain(&auth_sec_file);
	if (ret != 0) {
		pr_err("%s: failed to certify ELF signature & cert chain!\n", __func__);
		goto verify_kmod_auth_sec_done;
	}

	/*
	 * Certify hash segments
	 */
	auth_sec_shash = crypto_alloc_shash("sha1", 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(auth_sec_shash)) {
		pr_err("failed to alloc shash!\n");
		ret = PTR_ERR(auth_sec_shash);
		goto verify_kmod_auth_sec_done;
	}

	ret = auth_sec_cert_hash(&auth_sec_file, auth_sec_shash);
	if (ret != 0) {
		pr_err("%s: failed to certify ELF hash table!\n", __func__);
		goto verify_kmod_auth_sec_done;
	}

verify_kmod_auth_sec_done:

	if (auth_sec_shash != NULL && !IS_ERR(auth_sec_shash)) {
		crypto_free_shash(auth_sec_shash);
	}

	if (auth_sec_file.phdr != NULL) {
		vfree(auth_sec_file.phdr);
		auth_sec_file.phdr = NULL;
	}

	if (auth_sec_file.elfent != NULL) {
		vfree(auth_sec_file.elfent);
		auth_sec_file.elfent = NULL;
	}

	mutex_unlock(&auth_sec_lock);

	pr_debug("%s: x\n", __func__);

	return ret;
}
EXPORT_SYMBOL(verify_kmod_auth_sec);

/*
 * Load real Kmod after auth sec
 */
int32_t load_kmod_auth_sec(void __user **umod, unsigned long *len)
{
	int32_t ret = 0;

	pr_debug("%s: e\n", __func__);

	/*
	 * Sanity check
	 */
	if (*len < sizeof(Elf32_Ehdr)) {
		pr_err("%s: invalid values!\n", __func__);
		return -EINVAL;
	}

	mutex_lock(&auth_sec_lock);

	/*
	 * Populate ELF
	 */
	auth_sec_file.elfent_len = (uint32_t)(*len);
	auth_sec_file.elfent = (uint8_t *)vmalloc(auth_sec_file.elfent_len);
	if (auth_sec_file.elfent == NULL) {
		pr_err("%s: no enough memory!\n", __func__);
		mutex_unlock(&auth_sec_lock);
		return -ENOMEM;
	}

	if (copy_from_user((void *)auth_sec_file.elfent, (const void __user *)(*umod), (unsigned long)auth_sec_file.elfent_len) != 0) {
		pr_err("%s: failed to copy data from user!\n", __func__);
		ret = -EFAULT;
		goto load_kmod_auth_sec_done;
	}

	/*
	 * Populate ELF header
	 */
	memcpy((void *)&auth_sec_file.ehdr, (const void *)auth_sec_file.elfent, sizeof(Elf32_Ehdr));

	/*
	 * Populate ELF program header table
	 */
	auth_sec_file.phdr = (Elf32_Phdr *)vmalloc(auth_sec_file.ehdr.e_phentsize * auth_sec_file.ehdr.e_phnum);
	if (auth_sec_file.phdr == NULL) {
		pr_err("%s: no enough memory!\n", __func__);
		ret = -ENOMEM;
		goto load_kmod_auth_sec_done;
	}

	ret = auth_sec_populate_elf_phdr_tbl(&auth_sec_file, auth_sec_file.phdr);
	if (ret != 0) {
		pr_err("%s: failed to populate ELF program header table!\n", __func__);
		goto load_kmod_auth_sec_done;
	}

	/*
	 * Populate ELF code segment which is at index of AUTH_SEC_PHNUM - 1
	 */
	*umod = (uint8_t __force *)(*umod) + auth_sec_file.phdr[AUTH_SEC_PHNUM - 1].p_offset;
	*len = auth_sec_file.phdr[AUTH_SEC_PHNUM - 1].p_filesz;

load_kmod_auth_sec_done:

	if (auth_sec_file.phdr != NULL) {
		vfree(auth_sec_file.phdr);
		auth_sec_file.phdr = NULL;
	}

	if (auth_sec_file.elfent != NULL) {
		vfree(auth_sec_file.elfent);
		auth_sec_file.elfent = NULL;
	}

	mutex_unlock(&auth_sec_lock);

	pr_debug("%s: x\n", __func__);

	return ret;
}
EXPORT_SYMBOL(load_kmod_auth_sec);

/*
 * Show status of certificate 
 */
static ssize_t show_auth_sec_certst(struct sys_device *dev, struct sysdev_attribute *attr, char *buf)
{
	/*
	 * No sanity check here!
	 */
	return snprintf(buf, PAGE_SIZE, "%d\n", auth_sec_file.certst);
}
static SYSDEV_ATTR(certst, S_IRUSR, show_auth_sec_certst, NULL);

static struct sysdev_attribute *auth_sec_attrs[] = {
	&attr_certst,
};

static struct sysdev_class auth_sec_sysdev_class = {
	.name = AUTH_SEC_CLASS_NAME
};

/*
 * Sys device register
 *
 * sysdev file:
 *
 * /sys/devices/system/zte_kmod_auth_sec/zte_kmod_auth_sec0/certst
 */
static int32_t auth_sec_register_sysdev(struct sys_device *sysdev)
{
	int32_t i = 0;
	int32_t ret = 0;

	pr_debug("%s: e\n", __func__);

	ret = sysdev_class_register(&auth_sec_sysdev_class);
	if (ret) {
		pr_err("%s: failed to register sys class!\n", __func__);
		return ret;
	}

	sysdev->id = 0;
	sysdev->cls = &auth_sec_sysdev_class;

	ret = sysdev_register(sysdev);
	if (ret) {
		pr_err("%s: failed to register sys dev!\n", __func__);
		sysdev_class_unregister(&auth_sec_sysdev_class);
		return ret;
	}

	for (i = 0; i < ARRAY_SIZE(auth_sec_attrs); ++i) {
		ret = sysdev_create_file(sysdev, auth_sec_attrs[i]);
		if (ret) {
			pr_err("%s: failed to create sys dev file!\n", __func__);
			goto auth_sec_register_sysdev_fail;
		}
	}

	pr_debug("%s: x\n", __func__);

	return 0;

auth_sec_register_sysdev_fail:

	while (--i >= 0) sysdev_remove_file(sysdev, auth_sec_attrs[i]);

	sysdev_unregister(sysdev);
	sysdev_class_unregister(&auth_sec_sysdev_class);

	return ret;
}

#if 0 // disused here
static int32_t __init auth_sec_probe(struct platform_device *pdev)
{
	pr_debug("%s: e\n", __func__);
	pr_debug("%s: x\n", __func__);

	return 0;
}

static int32_t auth_sec_remove(struct platform_device *pdev)
{
	pr_debug("%s: e\n", __func__);
	pr_debug("%s: x\n", __func__);

	return 0;
}

static struct platform_driver auth_sec_driver = {
	.remove = auth_sec_remove,
	.driver = {
		.name = AUTH_SEC_CLASS_NAME,
		.owner = THIS_MODULE,
	},
};
#endif

/*
 * Initializes the module.
 */
static int32_t __init auth_sec_init(void)
{
	pr_debug("%s: e\n", __func__);

	/*
	 * Register device driver
	 */
#if 0 // disused here
	ret = platform_driver_probe(&auth_sec_driver, auth_sec_probe);
	if (ret) {
		pr_err("%s: failed to register driver!\n", __func__);
		return ret;
	}
#endif

	/*
	 * Register sys device
	 */
	(void)auth_sec_register_sysdev(&auth_sec_sysdev);

	pr_debug("%s: x\n", __func__);

	return 0;
}

/*
 * Cleans up the module.
 */
static void __exit auth_sec_exit(void)
{
	/*
	 * Unregister sysdev
	 */
	// Add code here

	/*
	 * Unregister device driver
	 */
#if 0 // disused here
	platform_driver_unregister(&auth_sec_driver);
#endif
}

module_init(auth_sec_init);
module_exit(auth_sec_exit);

MODULE_DESCRIPTION("ZTE Auth-Sec Module Ver %s" AUTH_SEC_VERSION);
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

