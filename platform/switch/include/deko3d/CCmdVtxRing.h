/*
** Sample Framework for deko3d Applications
**   CCmdMemRing.h: Memory provider class for dynamic command buffers
*/
#pragma once

#include "deko3d/common.h"
#include "deko3d/CMemPool.h"

template <unsigned NumSlices>
class CCmdVtxRing
{
    static_assert(NumSlices > 0, "Need a non-zero number of slices...");

    CMemPool::Handle m_mem;
    unsigned m_curSlice;
    uint32_t m_sliceSize;

    public:
        CCmdVtxRing() : m_mem{}, m_curSlice{} { }

        ~CCmdVtxRing()
        {
            m_mem.destroy();
        }

        bool allocate(CMemPool & pool, uint32_t size)
        {
            m_sliceSize = (size + DK_CMDMEM_ALIGNMENT - 1) &~ (DK_CMDMEM_ALIGNMENT - 1);
            m_mem = pool.allocate(NumSlices * m_sliceSize);

            return m_mem;
        }

        const uint32_t getSize()
        {
            return m_mem.getSize();
        }

        const unsigned getCurSlice()
        {
            return m_curSlice;
        }

        std::pair<void *, DkGpuAddr> begin()
        {
            return std::make_pair((void *)((char *)m_mem.getCpuAddr() + (m_curSlice * m_sliceSize)), m_mem.getGpuAddr());
        }

        void end()
        {
            // Advance the current slice counter; wrapping around when we reach the end
            m_curSlice = (m_curSlice + 1) % NumSlices;
        }
};