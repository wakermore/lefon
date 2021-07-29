#include <common.h>
#include <console.h>
#include <bootretry.h>
#include <cli.h>
#include <command.h>
#include <console.h>
#include <hash.h>
#include <inttypes.h>
#include <mapmem.h>
#include <watchdog.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <cmd_text.h>
#include <cmd_text.h>

DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_SYS_MEMTEST_SCRATCH
#define CONFIG_SYS_MEMTEST_SCRATCH 0
#endif

static int mod_mem(cmd_tbl_t *, int, int, int, char * const []);

/* Display values from last command.
 * Memory modify remembered values are different from display memory.
 */
static ulong	dp_last_addr, dp_last_size;
static ulong	dp_last_length = 0x40;
static ulong	mm_last_addr, mm_last_size;

static	ulong	base_address = 0;
#define DISP_LINE_LEN 16

static int do_mem_md(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	ulong	addr, length, bytes;
	const void *buf;
	int	size;
	int rc = 0;

	/* We use the last specified parameters, unless new ones are
	 * entered.
	 */
	addr = dp_last_addr;
	size = dp_last_size;
	length = dp_last_length;

	if (argc < 2)
		return CMD_RET_USAGE;

	if ((flag & CMD_FLAG_REPEAT) == 0) {
		/* New command specified.  Check for a size specification.
		 * Defaults to long if no or incorrect specification.
		 */
		if ((size = cmd_get_data_size(argv[0], 4)) < 0)
			return 1;

		/* Address is specified since argc > 1
		*/
		addr = simple_strtoul(argv[1], NULL, 16);
		addr += base_address;

		/* If another parameter, it is the length to display.
		 * Length is the number of objects, not number of bytes.
		 */
		if (argc > 2)
			length = simple_strtoul(argv[3], NULL, 16);
	}

	bytes = size * length;
	buf = map_sysmem(addr, bytes);

	/* Print the lines. */
	print_buffer(addr, buf, size, length, DISP_LINE_LEN / size);
	addr += bytes;
	unmap_sysmem(buf);

	dp_last_addr = addr;
	dp_last_length = length;
	dp_last_size = size;
	return (rc);
}

/*|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||写函数*/
static int do_mem_mw(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
	u64 writeval;
#else
	ulong writeval;
#endif
	ulong	addr, count;
	int	size;
	void *buf, *start;
	ulong bytes;

	if ((argc < 3) || (argc > 4))
		return CMD_RET_USAGE;

	/* Check for size specification.
	*/
	if ((size = cmd_get_data_size(argv[0], 4)) < 1)
		return 1;

	/* Address is specified since argc > 1
	*/
	addr = simple_strtoul(argv[1], NULL, 16);
	addr += base_address;

	/* Get the value to write.
	*/
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
	writeval = simple_strtoull(argv[2], NULL, 16);
#else
	writeval = simple_strtoul(argv[2], NULL, 16);
#endif

	/* Count ? */
	if (argc == 4) {
		count = simple_strtoul(argv[3], NULL, 16);
	} else {
		count = 1;
	}

	bytes = size * count;
	start = map_sysmem(addr, bytes);
	buf = start;
	while (count-- > 0) {
		if (size == 4)
			*((u32 *)buf) = (u32)writeval;
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
		else if (size == 8)
			*((u64 *)buf) = (u64)writeval;
#endif
		else if (size == 2)
			*((u16 *)buf) = (u16)writeval;
		else
			*((u8 *)buf) = (u8)writeval;
		buf += size;
	}
	unmap_sysmem(start);

/*开始读!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


do_mem_md(cmdtp,flag,argc,argv);	
}

U_BOOT_CMD(tx,5,1,do_mem_mw,"write and read""!!!!");
