/* Host counterpart of pio-scripts/dynarray.py: keep WLED's .dynarray.* sections (usermod
   registry) together and sorted, so DYNARRAY_BEGIN..DYNARRAY_END spans exactly the members. */
SECTIONS
{
  .dynarray : ALIGN(0x10) { KEEP(*(SORT(.dynarray.*))) }
}
INSERT AFTER .rodata;
