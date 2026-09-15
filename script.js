document.addEventListener("DOMContentLoaded", async () => {
    try {
        const manifestResponse = await fetch("code/c_manifest.json")
        
        if (!manifestResponse.ok) {
            throw new Error("Failed to load code/manifest.json")
        }
        
        const repositoryFileEntries = await manifestResponse.json()

        initializeInteractiveRepositoryTree(repositoryFileEntries)
        
    } catch (error) {
        console.error("Error loading repository manifest:", error)

        const repositoryTreeContainer = document.getElementById("repository-tree-container")

        if (repositoryTreeContainer) {
            repositoryTreeContainer.innerHTML = `<span class="has-text-danger">// Error loading manifest.json: ${error.message}</span>`
        }
    }
})

function initializeInteractiveRepositoryTree(fileEntriesList) {
    const repositoryTreeContainer = document.getElementById("repository-tree-container")
    const activeCodeContainer = document.getElementById("active-code-container")
    const activeFileLabel = document.getElementById("active-file-label")

    if (!repositoryTreeContainer) {
        return
    }

    if (fileEntriesList.length === 0) {
        repositoryTreeContainer.innerHTML = `<span class="has-text-grey">// No repository files found.</span>`

        return
    }

    const groupedDirectoryMap = {}

    fileEntriesList.forEach(fileEntry => {
        if (!groupedDirectoryMap[fileEntry.folderPath]) {
            groupedDirectoryMap[fileEntry.folderPath] = []
        }

        groupedDirectoryMap[fileEntry.folderPath].push(fileEntry)
    })

    repositoryTreeContainer.innerHTML = ""

    Object.keys(groupedDirectoryMap).forEach(folderPath => {
        const folderWrapperElement = document.createElement("div")

        folderWrapperElement.className = "mb-1"

        const folderHeaderElement = document.createElement("div")

        folderHeaderElement.className = "has-text-primary is-flex is-align-items-center py-1 px-2"
        folderHeaderElement.innerHTML = `<i class="fas fa-folder mr-2"></i><span>${folderPath}</span>`

        folderWrapperElement.appendChild(folderHeaderElement)

        const filesListContainer = document.createElement("div")

        filesListContainer.className = "ml-3"
        filesListContainer.style.borderLeft = "1px dashed var(--border-color)"

        groupedDirectoryMap[folderPath].forEach(fileEntry => {
            const fileItemButton = document.createElement("a")

            fileItemButton.className = "is-block py-1 px-2 has-text-grey-light tree-file-item"
            fileItemButton.style.borderRadius = "0"
            fileItemButton.style.transition = "background 0.2s ease, color 0.2s ease"
            fileItemButton.innerHTML = `<i class="fas fa-file-code mr-2 has-text-info"></i>${fileEntry.fileName} <span class="is-size-8 has-text-grey">(${fileEntry.fileCategory})</span>`

            fileItemButton.addEventListener("mouseenter", () => {
                fileItemButton.style.background = "rgba(255, 255, 255, 0.05)"
                fileItemButton.style.color = "var(--text-color)"
            })

            fileItemButton.addEventListener("mouseleave", () => {
                if (!fileItemButton.classList.contains("is-active-file")) {
                    fileItemButton.style.background = "transparent"
                    fileItemButton.style.color = "var(--text-grey-light)"
                }
            })

            fileItemButton.addEventListener("click", async () => {
                document.querySelectorAll(".tree-file-item").forEach(element => {
                    element.classList.remove("is-active-file")
                    element.style.background = "transparent"
                    element.style.color = "var(--text-grey-light)"
                })

                fileItemButton.classList.add("is-active-file")
                fileItemButton.style.background = "rgba(222, 192, 165, 0.15)"
                fileItemButton.style.color = "var(--text-color)"

                if (activeFileLabel) {
                    activeFileLabel.innerHTML = `<i class="fas fa-file-code mr-2"></i> ${fileEntry.fileName}`
                }
                
                if (activeCodeContainer) {
                    activeCodeContainer.textContent = `// Loading ${fileEntry.fileName}...`

                    await fetchAndHighlightSelectedFile(fileEntry.filePath, activeCodeContainer)
                }
            })

            filesListContainer.appendChild(fileItemButton)
        })

        folderWrapperElement.appendChild(filesListContainer)
        repositoryTreeContainer.appendChild(folderWrapperElement)
    })
}

async function fetchAndHighlightSelectedFile(filePath, codeContainerElement) {
    try {
        const fileResponse = await fetch(filePath)
        
        if (!fileResponse.ok) {
            throw new Error(`Failed to retrieve file contents: ${filePath}`)
        }

        const fileContentText = await fileResponse.text()
        
        codeContainerElement.textContent = fileContentText
        codeContainerElement.removeAttribute("data-highlighted")
        codeContainerElement.className = "language-c"

        hljs.highlightElement(codeContainerElement)
    } catch (error) {
        console.error("Error fetching file content:", error)

        codeContainerElement.textContent = `// ERROR: Could not load file from path -> ${filePath}\n// Verify that the file exists at this path relative to your web root.`
        codeContainerElement.removeAttribute("data-highlighted")
    }
}

function collapseAndClear() {
    const targetElement = document.getElementById("active-code-container")
    const activeFileLabelElement = document.getElementById("active-file-label")

    if (targetElement) {
        targetElement.textContent = "// No file selected."
        targetElement.removeAttribute("data-highlighted")
    }

    if (activeFileLabelElement) {
        activeFileLabelElement.innerHTML = `<i class="fas fa-file-code mr-2"></i> No file selected`
    }

    document.querySelectorAll(".tree-file-item").forEach(element => {
        element.classList.remove("is-active-file")
        element.style.background = "transparent"
        element.style.color = "var(--text-grey-light)"
    })
}