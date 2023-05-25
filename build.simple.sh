gcc -I/home/hoefkens/Development/Code/C/ZeroProcHot/headers/efi -I/usr/include/efi -I/usr/include/efi/x86_64 -I/home/hoefkens/Development/Code/C/ZeroProcHot/../edk2/MdePkg/Include/Library -fno-stack-protector -o $1 $1.c 
./$1

