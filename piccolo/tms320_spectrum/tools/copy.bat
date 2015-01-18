set dest_dir=..\DebugProteus
set cof_dest_dir=..\DebugProteus\Debug


For /R %dest_dir% %%G IN (*.c *.asm *.cof) do del "%%G" 


For /R ..\ %%G IN (*.c *.asm) do copy "%%G" %dest_dir%
For /R ..\ %%G IN (*.cof) do copy "%%G" %cof_dest_dir%