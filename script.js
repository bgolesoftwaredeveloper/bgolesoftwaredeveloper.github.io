document.addEventListener('DOMContentLoaded', () => {
    const modal = document.getElementById('image-viewer-modal')
    const modalImg = document.getElementById('modal-display-img')
    const closeElements = modal.querySelectorAll('.modal-background, .modal-close')
    const triggerButtons = document.querySelectorAll('.js-view-asset')

    triggerButtons.forEach(button => {
        button.addEventListener('click', (e) => {
            e.preventDefault()
            const imgSrc = button.getAttribute('data-target-img')
            if (imgSrc && !imgSrc.includes('YOUR_IMAGE_URL')) {
                modalImg.src = imgSrc
                modal.classList.add('is-active')
                document.documentElement.classList.add('is-clipped')
            }
        })
    })

    closeElements.forEach(element => {
        element.addEventListener('click', () => {
            modal.classList.remove('is-active')
            document.documentElement.classList.remove('is-clipped')
            modalImg.src = ""
        })
    })

    document.addEventListener('keydown', (e) => {
        if (e.key === 'Escape' && modal.classList.contains('is-active')) {
            modal.classList.remove('is-active')
            document.documentElement.classList.remove('is-clipped')
            modalImg.src = ""
        }
    })

    const galleryColumns = document.querySelectorAll('.art-gallery-container .columns .column')
    const topCloseBtn = document.getElementById('js-close-gallery-top')
    const itemsToShowByDefault = 0

    galleryColumns.forEach((col, index) => {
        if (index >= itemsToShowByDefault) {
            col.style.display = 'none'
            col.classList.add('gallery-hidden-item')
        }
    })

    const galleryContainer = document.querySelector('.art-gallery-container')
    const toggleWrapper = document.createElement('div')

    toggleWrapper.className = 'has-text-centered mt-4'
    toggleWrapper.innerHTML = `
        <button id="js-toggle-gallery" class="button is-primary is-outlined mono is-small">
            <span class="icon"><i class="fa-solid fa-chevron-down"></i></span>
            <span>View Designs Matrix</span>
        </button>
    `

    galleryContainer.appendChild(toggleWrapper)

    const toggleBtn = document.getElementById('js-toggle-gallery')

    let isExpanded = false

    const updateGalleryState = (expand) => {
        isExpanded = expand
        
        galleryColumns.forEach((col, index) => {
            if (index >= itemsToShowByDefault) {
                col.style.display = isExpanded ? 'block' : 'none'
            }
        })

        if (isExpanded) {
            toggleBtn.innerHTML = `
                <span class="icon"><i class="fa-solid fa-chevron-up"></i></span>
                <span>Collapse Designs Matrix</span>
            `

            if (topCloseBtn) {
                topCloseBtn.style.display = 'inline-flex'
            }
        } else {
            toggleBtn.innerHTML = `
                <span class="icon"><i class="fa-solid fa-chevron-down"></i></span>
                <span>View Designs Matrix</span>
            `

            if (topCloseBtn) {
                topCloseBtn.style.display = 'none'
            }
        }
    }

    toggleBtn.addEventListener('click', (event) => {
        event.preventDefault()
        const nextState = !isExpanded
        updateGalleryState(nextState)
        
        if (!nextState) {
            document.querySelector('.gallery-header').scrollIntoView({ behavior: 'smooth', block: 'start' })
        } else {
            document.querySelector('.art-gallery-container').scrollIntoView({ behavior: 'smooth', block: 'start' })
        }
    })

    if (topCloseBtn) {
        topCloseBtn.addEventListener('click', (event) => {
            event.preventDefault()
            updateGalleryState(false)
            document.querySelector('.gallery-header').scrollIntoView({ behavior: 'smooth', block: 'start' })
        })
    }
})