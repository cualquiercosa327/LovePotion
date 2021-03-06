/*
** Sample Framework for deko3d Applications
**   CExternalImage.h: Utility class for loading images from the filesystem
*/
#pragma once

#include "deko3d/common.h"
#include "deko3d/CMemPool.h"

#include <libpng16/png.h>
#include <turbojpeg.h>

#include "common/lmath.h"

class CImage
{
    dk::Image m_image;
    dk::ImageDescriptor m_descriptor;
    CMemPool::Handle m_mem;

    public:
        CImage() : m_image{},
                   m_descriptor{},
                   m_mem{}
        {}

        ~CImage()
        {
            m_mem.destroy();
        }

        constexpr operator bool() const
        {
            return m_mem;
        }

        constexpr dk::Image & get()
        {
            return m_image;
        }

        constexpr dk::ImageDescriptor const & getDescriptor() const
        {
            return m_descriptor;
        }

        bool load(CMemPool & imagePool, CMemPool & scratchPool, dk::Device device, dk::Queue transferQueue,
                  void * buffer, size_t size, int & width, int & height);

        bool loadEmptyPixels(CMemPool & imagePool, CMemPool & scratchPool, dk::Device device, dk::Queue queue,
                             uint32_t width, uint32_t height, DkImageFormat format, uint32_t flags = 0);

        bool replacePixels(CMemPool & scratchPool, dk::Device device, const void * data,
                           size_t size, dk::Queue transferQueue, const love::Rect & rect);

        bool loadMemory(CMemPool & imagePool, CMemPool & scratchPool, dk::Device device, dk::Queue transferQueue,
                        const void * data, uint32_t width, uint32_t height, DkImageFormat format, uint32_t flags = 0);

        size_t getFormatSize(DkImageFormat format);

        private:
            u32 * loadPNG(void * buffer, size_t size, int & width, int & height);

            u8 * loadJPG(void * buffer, size_t size, int & width, int & height);
};
