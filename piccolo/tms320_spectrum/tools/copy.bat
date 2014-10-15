set dest_dir=..\DebugProteus
del "%dest_dir%\*.*?"
For /R ..\ %%G IN (*.c *.cof *.asm) do copy "%%G" %dest_dir%