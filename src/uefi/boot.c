#include <stddef.h>

#include <efi/efi.h>
#include <efi/efilib.h>

#include "info.h"

static EFI_STATUS read_kernel(Info *info, void *buf)
{
    EFI_STATUS status;

    EFI_GUID sfs_guid = gEfiSimpleFileSystemProtocolGuid;
    UINTN handle_count;
    EFI_HANDLE *handle_buf;
    uefi_call_wrapper(gBS->LocateHandleBuffer, 5, ByProtocol, &sfs_guid, NULL, &handle_count, &handle_buf);

    EFI_FILE_PROTOCOL *kernel;
    size_t i = 0;
    for (; i < handle_count; i++)
    {
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *esp;
        EFI_FILE_PROTOCOL *root;
        EFI_FILE_PROTOCOL *boot;

        uefi_call_wrapper(gBS->HandleProtocol, 3, handle_buf[i], &sfs_guid, (void **)&esp);
        uefi_call_wrapper(esp->OpenVolume, 2, esp, &root);
        status = uefi_call_wrapper(root->Open, 5, root, &boot, L"boot", EFI_FILE_MODE_READ, 0);

        if (status == EFI_SUCCESS)
        {
            status = uefi_call_wrapper(boot->Open, 5, boot, &kernel, L"kernel.bin", EFI_FILE_MODE_READ, 0);
            if (status == EFI_SUCCESS)
                break;
        }
    }

    if (i == handle_count)
    {
        Print(L"/boot/kernel.bin file not found\n");
        return status;
    }

    EFI_FILE_INFO *kernel_info = NULL;
    EFI_GUID fi_guid = EFI_FILE_INFO_ID;
    UINTN file_info_size = 0;
    uefi_call_wrapper(kernel->GetInfo, 4, kernel, &fi_guid, &file_info_size, NULL);

    char kernel_info_buf[file_info_size];
    kernel_info = (EFI_FILE_INFO *)kernel_info_buf;
    uefi_call_wrapper(kernel->GetInfo, 4, kernel, &fi_guid, &file_info_size, &kernel_info_buf);

    Print(L"/boot/kernel.bin file size %lu bytes\n", kernel_info->FileSize);

    uefi_call_wrapper(kernel->Read, 3, kernel, &kernel_info->FileSize, buf);
    uefi_call_wrapper(kernel->Close, 1, kernel);
    uefi_call_wrapper(gBS->FreePool, 1, handle_buf);

    info->kernel_size = kernel_info->FileSize;
    return EFI_SUCCESS;
}

static EFI_STATUS get_memory_map(Memory_map *ret, UINTN *key)
{
    EFI_STATUS status;

    UINTN map_size = 0;
    UINTN map_key = 0;
    EFI_MEMORY_DESCRIPTOR *map = NULL;
    UINTN descriptor_size = 0;
    UINT32 descriptor_version = 0;
    uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, map, &map_key, &descriptor_size, &descriptor_version);

    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiBootServicesData, map_size, &map);

    if (status != EFI_SUCCESS)
    {
        Print(L"EfiBootServicesData memory allocation failed\n");
        return status;
    }

    uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, map, &map_key, &descriptor_size, &descriptor_version);

    ret->size = descriptor_size;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, ret->size * sizeof(Descriptor), &ret->descriptor);
    if (status != EFI_SUCCESS)
    {
        Print(L"EfiLoaderData memory allocation failed\n");
        return status;
    }

    for (size_t i = 0; i < descriptor_size; i++)
    {
        ret->descriptor[i].type = map[i].Type;
        ret->descriptor[i].pad = map[i].Pad;
        ret->descriptor[i].physical_start = map[i].PhysicalStart;
        ret->descriptor[i].virtual_start = map[i].VirtualStart;
        ret->descriptor[i].number_of_pages = map[i].NumberOfPages;
        ret->descriptor[i].attribute = map[i].Attribute;
    }

    uefi_call_wrapper(gBS->FreePool, 1, map);
    return EFI_SUCCESS;
}

static void get_graphics_output_protocol(Video_mode *ret)
{
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

    uefi_call_wrapper(BS->LocateProtocol, 3, &gop_guid, NULL, (void **)&gop);
    UINTN mode = gop->Mode->Mode;
    uefi_call_wrapper(gST->ConOut->ClearScreen, 1, gST->ConOut);
    uefi_call_wrapper(gop->SetMode, 2, gop, mode);

    Print(L"display screen resolution %d * %d\n", gop->Mode->Info->HorizontalResolution, gop->Mode->Info->VerticalResolution);

    ret->frame_buffer_base = (void *)gop->Mode->FrameBufferBase;
    ret->frame_buffer_size = gop->Mode->FrameBufferSize;
    ret->vertical_resolution = gop->Mode->Info->VerticalResolution;

    if (gop->Mode->Info->HorizontalResolution != gop->Mode->Info->PixelsPerScanLine)
    {
        Print(L"Please note that HorizontalResolution is not equal to PixelsPerScanLine. This may cause display output problems\n");
        ret->horizontal_resolution = gop->Mode->Info->PixelsPerScanLine;
    }
    else
        ret->horizontal_resolution = gop->Mode->Info->HorizontalResolution;
}

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table)
{
    EFI_STATUS staus;
    InitializeLib(image_handle, system_table);

    //获取信息
    Info info;
    get_graphics_output_protocol(&info.video_mode);

    UINTN key;
    staus = get_memory_map(&info.memory_map, &key);
    if (staus != EFI_SUCCESS)
        goto Error;

    //读取内核
    staus = read_kernel(&info, (void *)0x100000);
    if (staus != EFI_SUCCESS)
        goto Error;

    Print(L"Use this operating system at your own risk\n");
    Print(L"Hit any key on the keyboard to enter the operating system\n");
    uefi_call_wrapper(system_table->BootServices->WaitForEvent, 3, 1, &system_table->ConIn->WaitForKey, NULL);
    uefi_call_wrapper(BS->ExitBootServices, 2, image_handle, key);

    //跳转内核
    int (*main_kernel)(Info *) = (int (*)(Info *))0x100000;
    main_kernel(&info);

    //发生错误退出UEFI
Error:
    Print(L"A serious error occurred UEFI program exited\n");
    Print(L"Hit any key on the keyboard to exit UEFI\n");
    uefi_call_wrapper(system_table->BootServices->WaitForEvent, 3, 1, &system_table->ConIn->WaitForKey, NULL);

    return EFI_SUCCESS;
}
