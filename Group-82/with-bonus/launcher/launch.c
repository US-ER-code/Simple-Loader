#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include "loader.h"

//Function to check if its an ELF FILE
int check_elf(Elf32_Ehdr elf_hdr){
    if (elf_hdr.e_ident[EI_MAG0] != 0x7f || 
            elf_hdr.e_ident[EI_MAG1] != 'E' || 
            elf_hdr.e_ident[EI_MAG2] != 'L' || 
            elf_hdr.e_ident[EI_MAG3] != 'F') {
            fprintf(stderr, "Error: Magic Number are not correct.\n");
            return 0;
        }
    }

// Function to check if the file is a valid ELF executable
int check_executable(const char *filename) {
    int fd;
    Elf32_Ehdr elf_hdr;

    // Open the file
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Error to OPEN\n");
        return 0;
    }

    // Read the ELF header into elf_hdr
    if (read(fd, &elf_hdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)) {
        perror("Error in READING\n");
        close(fd);
        return 0;
    }

    // Check for magic numbers
    if(!check_elf(elf_hdr)){
        close(fd);
        return 0;
    }
    

    // Check if the ELF file is executable (e_type == ET_EXEC)
    if (elf_hdr.e_type != ET_EXEC) {
        fprintf(stderr, "Error: ELF file is not an executable.\n");
        close(fd);
        return 0;
    }

    close(fd);
    return 1;
}

int main(int argc, char** argv) {
    // Check for correct number of command line arguments
    if(argc != 2) {
        printf("Usage: %s <ELF Executable>\n", argv[0]);
        return 1;
    }

    // Validate the ELF file
    if (!check_executable(argv[1])) {
        return 1;  // Exit if the ELF file is invalid
    }

    // Step 1: Pass the ELF file to the loader to load and execute it
    load_and_run_elf(argv);

    // Step 2: Invoke the cleanup routine
    loader_cleanup();

    return 0;
}
