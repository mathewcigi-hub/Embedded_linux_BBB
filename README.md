# Embedded_linux_BBB


After setting up Beagleboard board to start from eMMC.

For making projects:
1. Eclipse
2. Cross compilation toolchain to generate binaries
3. 'Make' (for Windows)

Download "Remote system Explorer" to transfer the code on BBB

In windows:
Open eclipse
   * New-> Create c project -> Cross GCC -> Next -> Advanced settings -> Environment -> "PATH" (In here select the cross compliation toolchain) -> OK
   Click Next -> Select Cross Cimplier prefix and its path -> Finish

   * Select the project and click on "build" option to generate the binary
   * In "show views" -> other -> Remote system -> select 'Remote System'

Connect the BBB to PC via USB cable.
When reset the board will boot the debian image from the eMMC memory(This setup should be done before doing this)


Open tera term and login to beaglebone account

The file structure in the "Remote system" will look like this
  > Local
    > Local files
    > Local shells
  > BBB
    > Sftp files
      > My home
        > bin
        > Desktop    
      > Root
    > Ssh Shells     

Drag and drop the binary file at the desktop in the "My home" 
Run it in the Terminal(in elsipse) - make necessary gpio lines are output
/# ./counter_7seg up 500

   














