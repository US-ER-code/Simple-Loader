#include "loader.h"

Elf32_Ehdr *ehdr;   //elf header 

Elf32_Phdr *phdr;   //programe header
void *buff;         //void pointer 
int fd;             //file discriptor
void *SEG_CONTENT;   //VOID POINTER OF MEMORY SEGMENT



/*
 * release memory and other cleanups
 */
void loader_cleanup() {
  munmap(SEG_CONTENT, phdr->p_memsz);  
  free(phdr);
  free(buff);
}

/*
 * Load and run the ELF executable file
 */
 
void load_and_run_elf(char** argv) {
  fd = open(argv[1], O_RDONLY);
  // 1. Load entire binary content into the memory from the ELF file.
  //calculate the size of elf 
  
  off_t ELF_si = lseek(fd, 0, SEEK_END);  //ELF_si=size of elf 	
  //error handling
  
  if(ELF_si==-1 ){
      perror("error");
      close(fd);
      return ;
  }
  buff = calloc(1,ELF_si);				//dynamic memory allocation					//
  
  if (buff == NULL) {
        perror("malloc-ation failed.");
        close(fd);
        return ;
    }
    
    //seek to the begining
  off_t begin=lseek(fd, 0, SEEK_SET);
  if (begin == -1) {
        perror("Could not reset the pointer to 0.");
        free(buff);
        close(fd);
        return ;
    }
  
  ssize_t BYTES_red = read(fd, buff, ELF_si);	//read 
  
  //error handling 
  if(BYTES_red==-1){
      perror("error");
      close(fd);
      return ;
  }
   
  ehdr = (Elf32_Ehdr *)buff;									//typecast of buff
  
    
    
    
//now iterate thorugh phdr
// 2. Iterate through the PHDR table and find the section of PT_LOA
  phdr = calloc(1,ehdr->e_phentsize);
  int i =0;
  while(i < ehdr->e_phnum){
    lseek(fd, ehdr->e_phoff + i * ehdr->e_phentsize, SEEK_SET);
    read(fd, phdr, ehdr -> e_phentsize);
    if (phdr->p_type == PT_LOAD) {							
	
        Elf64_Addr segment_start = phdr->p_vaddr;
        Elf64_Addr segment_end = phdr->p_vaddr + phdr->p_memsz;
        Elf64_Addr entry_point = ehdr->e_entry;

        if (entry_point >= segment_start && entry_point < segment_end) {
            break;
        }

             
            
    
    } 
    i++;
      
  }
  
  if(i>= ehdr->e_phnum)  {
      perror("PT_LOAD segemnet IS FOUND ");
      exit(1);
  } 
  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content
  
  
  // SEG_CONTENT =to copy the content  of segment (retur type)
  
  
  SEG_CONTENT = mmap(NULL, phdr->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC,	//making dynamic memeory in heap
                                MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
                                
  // error handling                                  
                                
  if (SEG_CONTENT == MAP_FAILED) {         //error handling
    perror("mmap");
    close(fd);
    return ;
    } 
    
  // Set the file offset to the beginning of the segment
  if (lseek(fd, phdr->p_offset, SEEK_SET) == -1) {
    perror("lseek failed");
    // Handle the error appropriately, such as exiting or returning an error code
    exit(EXIT_FAILURE);
}

// Read the segment data into memory
  ssize_t bytes_read = read(fd, SEG_CONTENT, phdr->p_filesz);
    if (bytes_read == -1) {
      perror("read failed");
    // Handle the error appropriately, such as exiting or returning an error code
      exit(EXIT_FAILURE);
} 
  else if ((size_t)bytes_read != phdr->p_filesz) {
    fprintf(stderr, "Warning: Read fewer bytes than expected. Expected %zu, got %zd\n",
            phdr->p_filesz, bytes_read);
    // Handle partial read if necessary
}
  close(fd);//done with file handling
  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
    // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
    
  
  
  
  int (*_start)() = (int (*)())(SEG_CONTENT + (ehdr->e_entry - phdr->p_vaddr)); // The function pointer _start points to a function returning an int and taking no arguments.
  
  if (_start == NULL) {
      perror("Failed to assign function pointer _start");
      exit(EXIT_FAILURE);
  }
  
  int result = _start();
  
  if (result == -1) {
      perror("Function _start() returned an error");
      exit(EXIT_FAILURE);
  }
  
  printf("User _start return value = %d\n", result);

  
  
  }





int main(int argc, char** argv) 
{
  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n",argv[0]);
    exit(1);
  }
  // 1. carry out necessary checks on the input ELF file
  // 2. passing it to the loader for carrying out the loading/execution
  load_and_run_elf(argv);
  // 3. invoke the cleanup routine inside the loader  
  loader_cleanup();
  return 0;
}
