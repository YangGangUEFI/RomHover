
#include "Include/PeImage.h"




//
// Modifier to ensure that all protocol member functions and EFI intrinsics
// use the correct C calling convention. All protocol member functions and
// EFI intrinsics are required to modify their member functions with EFIAPI.
//
#ifdef EFIAPI
///
/// If EFIAPI is already defined, then we use that definition.
///
#elif defined (_MSC_EXTENSIONS)
///
/// Microsoft* compiler specific method for EFIAPI calling convention.
///
#define EFIAPI  __cdecl
#elif defined (__GNUC__)
///
/// Define the standard calling convention regardless of optimization level.
/// The GCC support assumes a GCC compiler that supports the EFI ABI. The EFI
/// ABI is much closer to the x64 Microsoft* ABI than standard x64 (x86-64)
/// GCC ABI. Thus a standard x64 (x86-64) GCC compiler can not be used for
/// x64. Warning the assembly code in the MDE x64 does not follow the correct
/// ABI for the standard x64 (x86-64) GCC.
///
#define EFIAPI
#else
///
/// The default for a non Microsoft* or GCC compiler is to assume the EFI ABI
/// is the standard.
///
#define EFIAPI
#endif


//
// Status codes common to all execution phases
//
typedef UINTN RETURN_STATUS;


//
// PeCoffLib.h
//

/**
  Reads contents of a PE/COFF image.

  A function of this type reads contents of the PE/COFF image specified by FileHandle. The read
  operation copies ReadSize bytes from the PE/COFF image starting at byte offset FileOffset into
  the buffer specified by Buffer. The size of the buffer actually read is returned in ReadSize.
  If FileOffset specifies an offset past the end of the PE/COFF image, a ReadSize of 0 is returned.
  A function of this type must be registered in the ImageRead field of a PE_COFF_LOADER_IMAGE_CONTEXT
  structure for the PE/COFF Loader Library service to function correctly.  This function abstracts access
  to a PE/COFF image so it can be implemented in an environment specific manner.  For example, SEC and PEI
  environments may access memory directly to read the contents of a PE/COFF image, and DXE or UEFI
  environments may require protocol services to read the contents of PE/COFF image
  stored on FLASH, disk, or network devices.

  If FileHandle is not a valid handle, then ASSERT().
  If ReadSize is NULL, then ASSERT().
  If Buffer is NULL, then ASSERT().

  @param  FileHandle      Pointer to the file handle to read the PE/COFF image.
  @param  FileOffset      Offset into the PE/COFF image to begin the read operation.
  @param  ReadSize        On input, the size in bytes of the requested read operation.
                          On output, the number of bytes actually read.
  @param  Buffer          Output buffer that contains the data read from the PE/COFF image.

  @retval RETURN_SUCCESS            The specified portion of the PE/COFF image was
                                    read and the size return in ReadSize.
  @retval RETURN_DEVICE_ERROR       The specified portion of the PE/COFF image
                                    could not be read due to a device error.

**/
typedef
RETURN_STATUS
(EFIAPI* PE_COFF_LOADER_READ_FILE)(
    IN     VOID* FileHandle,
    IN     UINTN  FileOffset,
    IN OUT UINTN* ReadSize,
    OUT    VOID* Buffer
    );

///
/// The context structure used while PE/COFF image is being loaded and relocated.
///
typedef struct {
    ///
    /// Set by PeCoffLoaderGetImageInfo() to the ImageBase in the PE/COFF header.
    ///
    PHYSICAL_ADDRESS            ImageAddress;
    ///
    /// Set by PeCoffLoaderGetImageInfo() to the SizeOfImage in the PE/COFF header.
    /// Image size includes the size of Debug Entry if it is present.
    ///
    UINT64                      ImageSize;
    ///
    /// Is set to zero by PeCoffLoaderGetImageInfo(). If DestinationAddress is non-zero,
    /// PeCoffLoaderRelocateImage() will relocate the image using this base address.
    /// If the DestinationAddress is zero, the ImageAddress will be used as the base
    /// address of relocation.
    ///
    PHYSICAL_ADDRESS            DestinationAddress;
    ///
    /// PeCoffLoaderLoadImage() sets EntryPoint to to the entry point of the PE/COFF image.
    ///
    PHYSICAL_ADDRESS            EntryPoint;
    ///
    /// Passed in by the caller to PeCoffLoaderGetImageInfo() and PeCoffLoaderLoadImage()
    /// to abstract accessing the image from the library.
    ///
    PE_COFF_LOADER_READ_FILE    ImageRead;
    ///
    /// Used as the FileHandle passed into the ImageRead function when it's called.
    ///
    VOID* Handle;
    ///
    /// Caller allocated buffer of size FixupDataSize that can be optionally allocated
    /// prior to calling PeCoffLoaderRelocateImage().
    /// This buffer is filled with the information used to fix up the image.
    /// The fixups have been applied to the image and this entry is just for information.
    ///
    VOID* FixupData;
    ///
    /// Set by PeCoffLoaderGetImageInfo() to the Section Alignment in the PE/COFF header.
    /// If the image is a TE image, then this field is set to 0.
    ///
    UINT32                      SectionAlignment;
    ///
    /// Set by PeCoffLoaderGetImageInfo() to offset to the PE/COFF header.
    /// If the PE/COFF image does not start with a DOS header, this value is zero.
    /// Otherwise, it's the offset to the PE/COFF header.
    ///
    UINT32                      PeCoffHeaderOffset;
    ///
    /// Set by PeCoffLoaderGetImageInfo() to the Relative Virtual Address of the debug directory,
    /// if it exists in the image
    ///
    UINT32                      DebugDirectoryEntryRva;
    ///
    /// Set by PeCoffLoaderLoadImage() to CodeView area of the PE/COFF Debug directory.
    ///
    VOID* CodeView;
    ///
    /// Set by PeCoffLoaderLoadImage() to point to the PDB entry contained in the CodeView area.
    /// The PdbPointer points to the filename of the PDB file used for source-level debug of
    /// the image by a debugger.
    ///
    CHAR8* PdbPointer;
    ///
    /// Is set by PeCoffLoaderGetImageInfo() to the Section Alignment in the PE/COFF header.
    ///
    UINTN                       SizeOfHeaders;
    ///
    /// Not used by this library class. Other library classes that layer on  top of this library
    /// class fill in this value as part of their GetImageInfo call.
    /// This allows the caller of the library to know what type of memory needs to be allocated
    /// to load and relocate the image.
    ///
    UINT32                      ImageCodeMemoryType;
    ///
    /// Not used by this library class. Other library classes that layer on top of this library
    /// class fill in this value as part of their GetImageInfo call.
    /// This allows the caller of the library to know what type of memory needs to be allocated
    /// to load and relocate the image.
    ///
    UINT32                      ImageDataMemoryType;
    ///
    /// Set by any of the library functions if they encounter an error.
    ///
    UINT32                      ImageError;
    ///
    /// Set by PeCoffLoaderLoadImage() to indicate the size of FixupData that the caller must
    /// allocate before calling PeCoffLoaderRelocateImage().
    ///
    UINTN                       FixupDataSize;
    ///
    /// Set by PeCoffLoaderGetImageInfo() to the machine type stored in the PE/COFF header.
    ///
    UINT16                      Machine;
    ///
    /// Set by PeCoffLoaderGetImageInfo() to the subsystem type stored in the PE/COFF header.
    ///
    UINT16                      ImageType;
    ///
    /// Set by PeCoffLoaderGetImageInfo() to TRUE if the PE/COFF image does not contain
    /// relocation information.
    ///
    BOOLEAN                     RelocationsStripped;
    ///
    /// Set by PeCoffLoaderGetImageInfo() to TRUE if the image is a TE image.
    /// For a definition of the TE Image format, see the Platform Initialization Pre-EFI
    /// Initialization Core Interface Specification.
    ///
    BOOLEAN                     IsTeImage;
    ///
    /// Set by PeCoffLoaderLoadImage() to the HII resource offset
    /// if the image contains a custom PE/COFF resource with the type 'HII'.
    /// Otherwise, the entry remains to be 0.
    ///
    PHYSICAL_ADDRESS            HiiResourceData;
    ///
    /// Private storage for implementation specific data.
    ///
    UINT64                      Context;

    // Extend
    UINT32                      NumberOfRvaAndSizes;
    EFI_IMAGE_DATA_DIRECTORY    DataDirectory[EFI_IMAGE_NUMBER_OF_DIRECTORY_ENTRIES];

} PE_COFF_LOADER_IMAGE_CONTEXT;



//
// UEFIBaseType.h
//

///
/// PE32+ Machine type for IA32 UEFI images.
///
#define EFI_IMAGE_MACHINE_IA32  0x014C

///
/// PE32+ Machine type for IA64 UEFI images.
///
#define EFI_IMAGE_MACHINE_IA64  0x0200

///
/// PE32+ Machine type for EBC UEFI images.
///
#define EFI_IMAGE_MACHINE_EBC  0x0EBC

///
/// PE32+ Machine type for X64 UEFI images.
///
#define EFI_IMAGE_MACHINE_X64  0x8664

///
/// PE32+ Machine type for ARM mixed ARM and Thumb/Thumb2 images.
///
#define EFI_IMAGE_MACHINE_ARMTHUMB_MIXED  0x01C2

///
/// PE32+ Machine type for AARCH64 A64 images.
///
#define EFI_IMAGE_MACHINE_AARCH64  0xAA64

///
/// PE32+ Machine type for RISC-V 32/64/128
///
#define EFI_IMAGE_MACHINE_RISCV32   0x5032
#define EFI_IMAGE_MACHINE_RISCV64   0x5064
#define EFI_IMAGE_MACHINE_RISCV128  0x5128

///
/// PE32+ Machine type for LoongArch 32/64 images.
///
#define EFI_IMAGE_MACHINE_LOONGARCH32  0x6232
#define EFI_IMAGE_MACHINE_LOONGARCH64  0x6264
