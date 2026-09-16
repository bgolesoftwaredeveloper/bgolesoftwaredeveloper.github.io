document.addEventListener("DOMContentLoaded", async () => {
    try {
        const cManifestResponse = await fetch("code/c_manifest.json")
        
        if (!cManifestResponse.ok) {
            throw new Error(`HTTP status ${cManifestResponse.status} loading code/c_manifest.json`)
        }
        
        const cRepositoryFileEntries = await cManifestResponse.json()
        initializeInteractiveRepositoryTree(cRepositoryFileEntries)
        
    } catch (error) {
        console.error("Error loading C repository manifest:", error)

        const cRepositoryTreeContainer = document.getElementById("repository-tree-container")

        if (cRepositoryTreeContainer) {
            cRepositoryTreeContainer.innerHTML = `<span class="has-text-danger">// Error loading c_manifest.json: ${error.message}</span>`
        }
    }

    try {
        const pythonManifestResponse = await fetch("code/python_manifest.json")
        
        if (!pythonManifestResponse.ok) {
            throw new Error(`HTTP status ${pythonManifestResponse.status} loading code/python_manifest.json`)
        }
        
        const pythonRepositoryFileEntries = await pythonManifestResponse.json()
        initializeInteractivePythonRepositoryTree(pythonRepositoryFileEntries)
        
    } catch (error) {
        console.error("Error loading Python repository manifest:", error)

        const pythonRepositoryTreeContainer = document.getElementById("python-repository-tree-container")

        if (pythonRepositoryTreeContainer) {
            pythonRepositoryTreeContainer.innerHTML = `<span class="has-text-danger"># Error loading python_manifest.json: ${error.message}</span>`
        }
    }

    collapseAndClear()
    collapseAndClearPython()
})

function initializeInteractiveRepositoryTree(fileEntriesList) {
    const repositoryTreeContainer = document.getElementById("repository-tree-container")
    const activeCodeContainer = document.getElementById("active-code-container")
    const activeFileLabel = document.getElementById("active-file-label")

    if (!repositoryTreeContainer) {
        return
    }

    if (!Array.isArray(fileEntriesList) || fileEntriesList.length === 0) {
        repositoryTreeContainer.innerHTML = `<span class="has-text-grey">// No C repository files found.</span>`
        return
    }

    const groupedDirectoryMap = {}

    fileEntriesList.forEach(fileEntry => {
        const folderName = fileEntry.folderPath || "Root"
        if (!groupedDirectoryMap[folderName]) {
            groupedDirectoryMap[folderName] = []
        }

        groupedDirectoryMap[folderName].push(fileEntry)
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

            fileItemButton.className = "is-block py-1 px-2 has-text-grey-light tree-file-item c-tree-file-item"
            fileItemButton.style.borderRadius = "0"
            fileItemButton.style.transition = "background 0.2s ease, color 0.2s ease"
            fileItemButton.innerHTML = `<i class="fas fa-file-code mr-2 has-text-info"></i>${fileEntry.fileName} <span class="is-size-8 has-text-grey">(${fileEntry.fileCategory || "Source"})</span>`

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
                document.querySelectorAll(".c-tree-file-item").forEach(element => {
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
                    expandCodeViewerContainer(activeCodeContainer)
                    activeCodeContainer.textContent = `// Loading ${fileEntry.fileName}...`
                    await fetchAndHighlightSelectedFile(fileEntry.filePath, activeCodeContainer, "language-c")
                }
            })

            filesListContainer.appendChild(fileItemButton)
        })

        folderWrapperElement.appendChild(filesListContainer)
        repositoryTreeContainer.appendChild(folderWrapperElement)
    })
}

function collapseAndClear() {
    const targetElement = document.getElementById("active-code-container")
    const activeFileLabelElement = document.getElementById("active-file-label")

    if (targetElement) {
        collapseCodeViewerContainer(targetElement)
    }

    if (activeFileLabelElement) {
        activeFileLabelElement.innerHTML = `<i class="fas fa-file-code mr-2"></i> No file selected`
    }

    document.querySelectorAll(".c-tree-file-item").forEach(element => {
        element.classList.remove("is-active-file")
        element.style.background = "transparent"
        element.style.color = "var(--text-grey-light)"
    })
}

function initializeInteractivePythonRepositoryTree(fileEntriesList) {
    const repositoryTreeContainer = document.getElementById("python-repository-tree-container")
    const activeCodeContainer = document.getElementById("python-active-code-container")
    const activeFileLabel = document.getElementById("python-active-file-label")

    if (!repositoryTreeContainer) {
        return
    }

    if (!Array.isArray(fileEntriesList) || fileEntriesList.length === 0) {
        repositoryTreeContainer.innerHTML = `<span class="has-text-grey"># No Python repository files found.</span>`
        return
    }

    const groupedDirectoryMap = {}

    fileEntriesList.forEach(fileEntry => {
        const folderName = fileEntry.folderPath || "Root"
        if (!groupedDirectoryMap[folderName]) {
            groupedDirectoryMap[folderName] = []
        }

        groupedDirectoryMap[folderName].push(fileEntry)
    })

    repositoryTreeContainer.innerHTML = ""

    Object.keys(groupedDirectoryMap).forEach(folderPath => {
        const folderWrapperElement = document.createElement("div")
        folderWrapperElement.className = "mb-1"

        const folderHeaderElement = document.createElement("div")
        folderHeaderElement.className = "has-text-warning is-flex is-align-items-center py-1 px-2"
        folderHeaderElement.innerHTML = `<i class="fas fa-folder mr-2"></i><span>${folderPath}</span>`

        folderWrapperElement.appendChild(folderHeaderElement)

        const filesListContainer = document.createElement("div")
        filesListContainer.className = "ml-3"
        filesListContainer.style.borderLeft = "1px dashed var(--border-color)"

        groupedDirectoryMap[folderPath].forEach(fileEntry => {
            const fileItemButton = document.createElement("a")

            fileItemButton.className = "is-block py-1 px-2 has-text-grey-light tree-file-item python-tree-file-item"
            fileItemButton.style.borderRadius = "0"
            fileItemButton.style.transition = "background 0.2s ease, color 0.2s ease"
            fileItemButton.innerHTML = `<i class="fab fa-python mr-2 has-text-warning"></i>${fileEntry.fileName} <span class="is-size-8 has-text-grey">(${fileEntry.fileCategory || "Script"})</span>`

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
                document.querySelectorAll(".python-tree-file-item").forEach(element => {
                    element.classList.remove("is-active-file")
                    element.style.background = "transparent"
                    element.style.color = "var(--text-grey-light)"
                })

                fileItemButton.classList.add("is-active-file")
                fileItemButton.style.background = "rgba(222, 192, 165, 0.15)"
                fileItemButton.style.color = "var(--text-color)"

                if (activeFileLabel) {
                    activeFileLabel.innerHTML = `<i class="fab fa-python mr-2"></i> ${fileEntry.fileName}`
                }
                
                if (activeCodeContainer) {
                    expandCodeViewerContainer(activeCodeContainer)
                    activeCodeContainer.textContent = `# Loading ${fileEntry.fileName}...`
                    await fetchAndHighlightSelectedFile(fileEntry.filePath, activeCodeContainer, "language-python")
                }
            })

            filesListContainer.appendChild(fileItemButton)
        })

        folderWrapperElement.appendChild(filesListContainer)
        repositoryTreeContainer.appendChild(folderWrapperElement)
    })
}

function collapseAndClearPython() {
    const targetElement = document.getElementById("python-active-code-container")
    const activeFileLabelElement = document.getElementById("python-active-file-label")

    if (targetElement) {
        collapseCodeViewerContainer(targetElement)
    }

    if (activeFileLabelElement) {
        activeFileLabelElement.innerHTML = `<i class="fas fa-file-code mr-2"></i> No file selected`
    }

    document.querySelectorAll(".python-tree-file-item").forEach(element => {
        element.classList.remove("is-active-file")
        element.style.background = "transparent"
        element.style.color = "var(--text-grey-light)"
    })
}

function collapseCodeViewerContainer(codeContainerElement) {
    codeContainerElement.textContent = ""
    codeContainerElement.removeAttribute("data-highlighted")
    const parentWrapperElement = codeContainerElement.closest(".code-viewer-wrapper")
    if (parentWrapperElement) {
        parentWrapperElement.style.display = "none"
    }
}

function expandCodeViewerContainer(codeContainerElement) {
    const parentWrapperElement = codeContainerElement.closest(".code-viewer-wrapper")
    if (parentWrapperElement) {
        parentWrapperElement.style.display = "block"
    }
}

async function fetchAndHighlightSelectedFile(filePath, codeContainerElement, languageClass = "language-c") {
    try {
        if (!filePath) {
            throw new Error("Target file path is undefined or null.")
        }

        const encodedFilePath = encodeURI(filePath)
        const fileResponse = await fetch(encodedFilePath)
        
        if (!fileResponse.ok) {
            throw new Error(`HTTP ${fileResponse.status} (${fileResponse.statusText}) attempting to fetch: ${filePath}`)
        }

        const fileContentText = await fileResponse.text()
        
        expandCodeViewerContainer(codeContainerElement)
        codeContainerElement.textContent = fileContentText
        codeContainerElement.removeAttribute("data-highlighted")
        codeContainerElement.className = languageClass

        if (window.hljs && typeof window.hljs.highlightElement === "function") {
            window.hljs.highlightElement(codeContainerElement)
        }
    } catch (error) {
        console.error("Error fetching file content:", error)

        expandCodeViewerContainer(codeContainerElement)
        const commentPrefix = languageClass === "language-python" ? "#" : "//"
        codeContainerElement.textContent = `${commentPrefix} ERROR: Could not load file from path -> ${filePath}\n${commentPrefix} Details: ${error.message}\n${commentPrefix} Verify that the file exists at this exact path relative to your web root.`
        codeContainerElement.removeAttribute("data-highlighted")
    }
}