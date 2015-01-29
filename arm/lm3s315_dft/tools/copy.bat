set dest_dir=..\Proteus
set elf_dest_dir=..\Proteus\elf


For /R %dest_dir% %%G IN (*.c *.asm *.elf) do del "%%G" 


::For /R ..\ %%G IN (*.c *.asm) do copy "%%G" %dest_dir%
For /R ..\ %%G IN (*.elf) do copy "%%G" %elf_dest_dir%