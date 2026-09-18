document.addEventListener("DOMContentLoaded", async () => {
    const repositoryConfigurations = [
        {
            manifestPath: "code/c_manifest.json",
            baseDirectory: "code/c",
            treeContainerId: "repository-tree-container",
            codeContainerId: "active-code-container",
            fileLabelId: "active-file-label",
            itemClass: "c-tree-file-item",
            folderThemeClass: "has-text-primary",
            iconClass: "fas fa-file-code has-text-info"
        },
        {
            manifestPath: "code/python_manifest.json",
            baseDirectory: "code/python",
            treeContainerId: "python-repository-tree-container",
            codeContainerId: "python-active-code-container",
            fileLabelId: "python-active-file-label",
            itemClass: "python-tree-file-item",
            folderThemeClass: "has-text-warning",
            iconClass: "fab fa-python has-text-warning"
        },
        {
            manifestPath: "code/go_manifest.json",
            baseDirectory: "code/golang",
            treeContainerId: "golang-repository-tree-container",
            codeContainerId: "golang-active-code-container",
            fileLabelId: "golang-active-file-label",
            itemClass: "golang-tree-file-item",
            folderThemeClass: "has-text-info",
            iconClass: "fa-brands fa-golang has-text-info"
        },
        {
            manifestPath: "code/under_the_hood_manifest.json",
            baseDirectory: "code/under-the-hood-ml",
            treeContainerId: "under-the-hood-repository-tree-container",
            codeContainerId: "under-the-hood-active-code-container",
            fileLabelId: "under-the-hood-active-file-label",
            itemClass: "under-the-hood-tree-file-item",
            folderThemeClass: "has-text-success",
            iconClass: "fas fa-gear has-text-success"
        }
    ]

    for (const repositoryConfiguration of repositoryConfigurations) {
        await initializeRepository(repositoryConfiguration)
    }
})

async function initializeRepository(repositoryConfiguration) {
    const treeContainerElement = document.getElementById(repositoryConfiguration.treeContainerId)
    const activeCodeContainer = document.getElementById(repositoryConfiguration.codeContainerId)
    const activeFileLabelElement = document.getElementById(repositoryConfiguration.fileLabelId)

    if (activeCodeContainer) {
        activeCodeContainer.textContent = ""
        activeCodeContainer.removeAttribute("data-highlighted")
        
        const parentWrapperElement = activeCodeContainer.closest(".code-viewer-wrapper")

        if (parentWrapperElement) {
            parentWrapperElement.style.display = "none"
        }
    }

    if (activeFileLabelElement) {
        activeFileLabelElement.innerHTML = `<i class="fas fa-file-code mr-2"></i> No file selected`
    }

    if (!treeContainerElement) {
        return
    }

    try {
        const manifestResponse = await fetch(repositoryConfiguration.manifestPath)

        if (!manifestResponse.ok) {
            throw new Error(`HTTP status ${manifestResponse.status} loading ${repositoryConfiguration.manifestPath}`)
        }

        const rawRepositoryFileEntries = await manifestResponse.json()

        if (!Array.isArray(rawRepositoryFileEntries) || rawRepositoryFileEntries.length === 0) {
            treeContainerElement.innerHTML = `<span class="has-text-grey">// No repository files found.</span>`
            return
        }

        const ignoredExtensions = [
            ".dll", ".exe", ".pdb", ".csproj", ".sln", ".slnx", ".user", ".suo", 
            ".cache", ".json", ".o", ".obj", ".a", ".so", ".dylib", ".pyc",
            ".jar", ".war", ".ear", ".class", ".pack", ".idx", "go.mod", "go.sum"
        ]
        const ignoredFolderPatterns = [
            "/obj/", "/bin/", "\\obj\\", "\\bin\\", ".vs/",
            "/lib/", "/libs/", "\\lib\\", "\\libs\\",
            ".git/", ".git\\", "/.git", "\\.git"
        ]

        const filteredRepositoryFileEntries = rawRepositoryFileEntries.filter(fileEntry => {
            const fileName = (fileEntry.fileName || "").toLowerCase()
            const filePath = (fileEntry.filePath || "").toLowerCase()
            const folderPath = (fileEntry.folderPath || "").toLowerCase()

            const hasIgnoredExtension = ignoredExtensions.some(extension => fileName.endsWith(extension))
            const isInIgnoredFolder = ignoredFolderPatterns.some(pattern => 
                filePath.includes(pattern) || folderPath.includes(pattern)
            )

            return !hasIgnoredExtension && !isInIgnoredFolder
        })

        if (filteredRepositoryFileEntries.length === 0) {
            treeContainerElement.innerHTML = `<span class="has-text-grey">// No source files found after filtering build artifacts.</span>`

            return
        }

        const groupedDirectoryMap = {}

        filteredRepositoryFileEntries.forEach(fileEntry => {
            const folderName = fileEntry.folderPath || "Root"

            if (!groupedDirectoryMap[folderName]) {
                groupedDirectoryMap[folderName] = []
            }

            groupedDirectoryMap[folderName].push(fileEntry)
        })

        treeContainerElement.innerHTML = ""

        Object.keys(groupedDirectoryMap).forEach(folderPath => {
            const folderWrapperElement = document.createElement("div")

            folderWrapperElement.className = "mb-1"

            const folderHeaderElement = document.createElement("div")

            folderHeaderElement.className = `${repositoryConfiguration.folderThemeClass} is-flex is-align-items-center py-1 px-2`
            folderHeaderElement.innerHTML = `<i class="fas fa-folder mr-2"></i><span>${folderPath}</span>`
            folderWrapperElement.appendChild(folderHeaderElement)

            const filesListContainer = document.createElement("div")

            filesListContainer.className = "ml-3"
            filesListContainer.style.borderLeft = "1px dashed var(--border-color)"

            groupedDirectoryMap[folderPath].forEach(fileEntry => {
                const fileItemButton = document.createElement("a")

                fileItemButton.className = `is-block py-1 px-2 has-text-grey-light tree-file-item ${repositoryConfiguration.itemClass}`
                fileItemButton.style.borderRadius = "0"
                fileItemButton.style.transition = "background 0.2s ease, color 0.2s ease"
                fileItemButton.innerHTML = `<i class="${repositoryConfiguration.iconClass} mr-2"></i>${fileEntry.fileName} <span class="is-size-8 has-text-grey">(${fileEntry.fileCategory || "Source"})</span>`

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
                    document.querySelectorAll(`.${repositoryConfiguration.itemClass}`).forEach(element => {
                        element.classList.remove("is-active-file")
                        element.style.background = "transparent"
                        element.style.color = "var(--text-grey-light)"
                    })

                    fileItemButton.classList.add("is-active-file")
                    fileItemButton.style.background = "rgba(222, 192, 165, 0.15)"
                    fileItemButton.style.color = "var(--text-color)"

                    if (activeFileLabelElement) {
                        activeFileLabelElement.innerHTML = `<i class="${repositoryConfiguration.iconClass} mr-2"></i> ${fileEntry.fileName}`
                    }

                    if (activeCodeContainer) {
                        await fetchAndDisplaySourceFile(fileEntry, activeCodeContainer, repositoryConfiguration.baseDirectory)
                    }
                })

                filesListContainer.appendChild(fileItemButton)
            })

            folderWrapperElement.appendChild(filesListContainer)
            treeContainerElement.appendChild(folderWrapperElement)
        })
    } catch (error) {
        console.error(`Error initializing repository (${repositoryConfiguration.manifestPath}):`, error)

        treeContainerElement.innerHTML = `<span class="has-text-danger">// Error loading manifest: ${error.message}</span>`
    }
}

async function fetchAndDisplaySourceFile(fileEntry, codeContainerElement, baseDirectory) {
    const parentWrapperElement = codeContainerElement.closest(".code-viewer-wrapper")

    if (parentWrapperElement) {
        parentWrapperElement.style.display = "block"
    }

    codeContainerElement.textContent = `// Loading ${fileEntry.fileName}...`

    let languageClass = "language-plaintext"

    if (fileEntry.fileName) {
        const lowercaseFileName = fileEntry.fileName.toLowerCase()

        if (lowercaseFileName.endsWith(".c") || lowercaseFileName.endsWith(".h")) {
            languageClass = "language-c"
        } else if (lowercaseFileName.endsWith(".cpp") || lowercaseFileName.endsWith(".hpp") || lowercaseFileName.endsWith(".cc") || lowercaseFileName.endsWith(".cxx")) {
            languageClass = "language-cpp"
        } else if (lowercaseFileName.endsWith(".cs")) {
            languageClass = "language-csharp"
        } else if (lowercaseFileName.endsWith(".py") || lowercaseFileName.endsWith(".pyx") || lowercaseFileName.endsWith(".pxd") || lowercaseFileName.endsWith(".pyi")) {
            languageClass = "language-python"
        } else if (lowercaseFileName.endsWith(".java")) {
            languageClass = "language-java"
        } else if (lowercaseFileName.endsWith(".go")) {
            languageClass = "language-go"
        } else if (lowercaseFileName.endsWith(".sh") || lowercaseFileName.endsWith(".bash")) {
            languageClass = "language-bash"
        }
    }

    try {
        if (!fileEntry.filePath) {
            throw new Error("Target file path is undefined or null.")
        }

        let resolvedFilePath = fileEntry.filePath.trim()

        if (resolvedFilePath.startsWith("/")) {
            resolvedFilePath = resolvedFilePath.slice(1)
        }

        if (resolvedFilePath.startsWith("code/darwin-systems/") || resolvedFilePath.startsWith("code/python-mac-os/")) {
            // Serve directly as define in manifest.
        } else {
            const cleanBaseDirectory = baseDirectory ? (baseDirectory.endsWith("/") ? baseDirectory.slice(0, -1) : baseDirectory) : ""

            if (cleanBaseDirectory) {
                if (!resolvedFilePath.startsWith(cleanBaseDirectory + "/")) {
                    const baseName = cleanBaseDirectory.split("/").pop()

                    if (resolvedFilePath.startsWith("code/") && !resolvedFilePath.startsWith(`code/${baseName}/`)) {
                        resolvedFilePath = resolvedFilePath.replace(/^code\//, `${cleanBaseDirectory}/`)
                    } else {
                        resolvedFilePath = `${cleanBaseDirectory}/${resolvedFilePath}`
                    }
                }
            }
        }

        const encodedFilePath = encodeURI(resolvedFilePath)
        const fileResponse = await fetch(encodedFilePath)

        if (!fileResponse.ok) {
            throw new Error(`HTTP ${fileResponse.status} (${fileResponse.statusText}) attempting to fetch: ${resolvedFilePath}`)
        }

        const fileContentText = await fileResponse.text()

        codeContainerElement.textContent = fileContentText
        codeContainerElement.removeAttribute("data-highlighted")
        codeContainerElement.className = languageClass

        if (window.hljs && typeof window.hljs.highlightElement === "function") {
            window.hljs.highlightElement(codeContainerElement)
        }
    } catch (error) {
        console.error("Error fetching file content:", error)

        const commentPrefix = languageClass === "language-python" ? "#" : "//"

        codeContainerElement.textContent = `${commentPrefix} ERROR: Could not load file from path -> ${fileEntry.filePath}\n${commentPrefix} Details: ${error.message}\n${commentPrefix} Verify that the file exists at this exact path relative to your web root.`
        codeContainerElement.removeAttribute("data-highlighted")
    }
}

function collapseAndClear() {
    resetRepositoryViewerState("active-code-container", "active-file-label", "c-tree-file-item")
}

function collapseAndClearPython() {
    resetRepositoryViewerState("python-active-code-container", "python-active-file-label", "python-tree-file-item")
}

function collapseAndClearGolang() {
    resetRepositoryViewerState("golang-active-code-container", "golang-active-file-label", "golang-tree-file-item")
}

function collapseAndClearUnderTheHood() {
    resetRepositoryViewerState("under-the-hood-active-code-container", "under-the-hood-active-file-label", "under-the-hood-tree-file-item")
}

function resetRepositoryViewerState(codeContainerId, fileLabelId, itemClass) {
    const activeCodeContainer = document.getElementById(codeContainerId)
    const activeFileLabelElement = document.getElementById(fileLabelId)

    if (activeCodeContainer) {
        activeCodeContainer.textContent = ""
        activeCodeContainer.removeAttribute("data-highlighted")

        const parentWrapperElement = activeCodeContainer.closest(".code-viewer-wrapper")

        if (parentWrapperElement) {
            parentWrapperElement.style.display = "none"
        }
    }

    if (activeFileLabelElement) {
        activeFileLabelElement.innerHTML = `<i class="fas fa-file-code mr-2"></i> No file selected`
    }

    document.querySelectorAll(`.${itemClass}`).forEach(element => {
        element.classList.remove("is-active-file")
        element.style.background = "transparent"
        element.style.color = "var(--text-grey-light)"
    })
}
