// Configure manual scroll restoration to force the browser to the top on page refresh.
if ('scrollRestoration' in history) {
    history.scrollRestoration = 'manual'
}

// Listen for the DOM content to fully load before initializing repository trees.
document.addEventListener("DOMContentLoaded", async () => {
    // Force scroll position to the top of the window on load or refresh.
    window.scrollTo(0, 0)

    // Define configuration objects for each managed repository source.
    const repositoryConfigurations = [
        {
            manifestPath: "code/c_manifest.json",
            baseDirectory: "code/c",
            treeContainerId: "repository-tree-container",
            codeContainerId: "active-code-container",
            fileLabelId: "active-file-label",
            viewerWrapperId: "c-code-viewer",
            itemClass: "c-tree-file-item",
            folderThemeClass: "has-text-success",
            iconClass: "fas fa-file-code has-text-success"
        },
        {
            manifestPath: "code/python_manifest.json",
            baseDirectory: "code/python",
            treeContainerId: "python-repository-tree-container",
            codeContainerId: "python-active-code-container",
            fileLabelId: "python-active-file-label",
            viewerWrapperId: "python-code-viewer",
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
            viewerWrapperId: "golang-code-viewer",
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
            viewerWrapperId: "under-the-hood-code-viewer",
            itemClass: "under-the-hood-tree-file-item",
            folderThemeClass: "has-text-primary",
            iconClass: "fas fa-gear has-text-primary"
        }
    ]

    // Loop through each configuration and initialize the respective repository viewer.
    for (const repositoryConfiguration of repositoryConfigurations) {
        await initializeRepository(repositoryConfiguration)
    }
})

/**
 * Determines the appropriate FontAwesome icon class based on the file extension.
 * @param {string} fileName - The name of the file.
 * @param {string} defaultIcon - The fallback icon class.
 * @returns {string} The CSS class string for the icon.
 */
function getFileIconForExtension(fileName, defaultIcon) {
    // Return the default icon if the file name is missing.
    if (!fileName) {
        return defaultIcon
    }

    // Convert the file name to lowercase for reliable extension matching.
    const lower = fileName.toLowerCase()

    // Return python-themed icon for python-related extensions.
    if (lower.endsWith(".py") || lower.endsWith(".pyx") || lower.endsWith(".pyi") || lower.endsWith(".pxd")) {
        return "fab fa-python has-text-warning"
    }

    // Return golang-themed icon for go source files.
    if (lower.endsWith(".go")) {
        return "fa-brands fa-golang has-text-info"
    }

    // Return csharp-themed icon for csharp source files.
    if (lower.endsWith(".cs")) {
        return "fas fa-code has-text-primary"
    }

    // Return java-themed icon for java source files.
    if (lower.endsWith(".java")) {
        return "fab fa-java has-text-danger"
    }

    // Return c/cpp-themed icon for c and c++ source or header files.
    if (lower.endsWith(".cpp") || lower.endsWith(".hpp") || lower.endsWith(".cc") || lower.endsWith(".c") || lower.endsWith(".h")) {
        return "fas fa-file-code has-text-success"
    }

    // Return the default icon if no specific extension match is found.
    return defaultIcon
}

/**
 * Fetches a repository manifest, filters out ignored files/folders, and builds the UI file tree.
 * @param {Object} repositoryConfiguration - Configuration parameters for the specific repository.
 */
async function initializeRepository(repositoryConfiguration) {
    // Retrieve DOM elements associated with the current repository configuration.
    const treeContainerElement = document.getElementById(repositoryConfiguration.treeContainerId)
    const activeCodeContainer = document.getElementById(repositoryConfiguration.codeContainerId)
    const activeFileLabelElement = document.getElementById(repositoryConfiguration.fileLabelId)

    // Reset the active code container and hide its parent wrapper on initialization.
    if (activeCodeContainer) {
        activeCodeContainer.textContent = ""
        activeCodeContainer.removeAttribute("data-highlighted")

        const parentWrapperElement = activeCodeContainer.closest(".code-viewer-wrapper")

        if (parentWrapperElement) {
            parentWrapperElement.style.display = "none"
        }
    }

    // Reset the active file label text to indicate no file is currently selected.
    if (activeFileLabelElement) {
        activeFileLabelElement.innerHTML = `<i class="fas fa-file-code mr-2"></i> No file selected`
    }

    // Exit early if the tree container element does not exist in the DOM.
    if (!treeContainerElement) {
        return
    }

    try {
        // Fetch the repository file manifest from the configured path.
        const manifestResponse = await fetch(repositoryConfiguration.manifestPath)

        // Throw an error if the manifest request fails.
        if (!manifestResponse.ok) {
            throw new Error(`HTTP status ${manifestResponse.status}`)
        }

        // Parse the JSON response containing raw repository file entries.
        const rawRepositoryFileEntries = await manifestResponse.json()

        // Display a fallback message if the manifest is empty or not an array.
        if (!Array.isArray(rawRepositoryFileEntries) || rawRepositoryFileEntries.length === 0) {
            treeContainerElement.innerHTML = `<span class="has-text-grey">// No repository files found.</span>`
            return
        }

        // Define a list of file extensions that should be ignored globally.
        let ignoredExtensions = [
            ".dll", ".exe", ".pdb", ".csproj", ".sln", ".slnx", ".user", ".suo",
            ".cache", ".json", ".o", ".obj", ".pyc", ".jar", "go.mod", "go.sum"
        ]

        // Add shared object (.so) files to the ignore list specifically for the machine learning repository.
        if (repositoryConfiguration.baseDirectory === "code/under-the-hood-ml") {
            ignoredExtensions.push(".so")
        }

        // Define folder path patterns that should be filtered out.
        const ignoredFolderPatterns = ["/obj/", "/bin/", ".vs/", "/lib/", ".git/"]

        // Filter the raw file entries based on ignored extensions, folder patterns, and specific files.
        const filteredRepositoryFileEntries = rawRepositoryFileEntries.filter(fileEntry => {
            const fileName = (fileEntry.fileName || "").toLowerCase()
            const filePath = (fileEntry.filePath || "").toLowerCase()
            const folderPath = (fileEntry.folderPath || "").toLowerCase()

            const hasIgnoredExtension = ignoredExtensions.some(ext => fileName.endsWith(ext))
            const isInIgnoredFolder = ignoredFolderPatterns.some(pat => filePath.includes(pat) || folderPath.includes(pat))

            let isIgnoredSpecificFile = false

            // Exclude specific testing framework files from the machine learning repository.
            if (repositoryConfiguration.baseDirectory === "code/under-the-hood-ml") {
                if (fileName === "unity.h" || fileName === "unity.c" || fileName === "unity_internals.h") {
                    isIgnoredSpecificFile = true
                }
            }

            return !hasIgnoredExtension && !isInIgnoredFolder && !isIgnoredSpecificFile
        })

        // Group the filtered file entries by their directory path.
        const groupedDirectoryMap = {}

        filteredRepositoryFileEntries.forEach(fileEntry => {
            const folderName = fileEntry.folderPath || "Root"

            if (!groupedDirectoryMap[folderName]) {
                groupedDirectoryMap[folderName] = []
            }

            groupedDirectoryMap[folderName].push(fileEntry)
        })

        // Clear the tree container element before rendering new folders and files.
        treeContainerElement.innerHTML = ""

        // Iterate over each grouped directory to build the UI elements.
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

            // Iterate over files in the current directory and create interactive item elements.
            groupedDirectoryMap[folderPath].forEach(fileEntry => {
                const fileItemButton = document.createElement("a")
                const fileIconClass = getFileIconForExtension(fileEntry.fileName, repositoryConfiguration.iconClass)

                fileItemButton.className = `is-block py-1 px-2 tree-file-item ${repositoryConfiguration.itemClass}`
                fileItemButton.style.borderRadius = "0"
                fileItemButton.style.transition = "background 0.2s ease, color 0.2s ease"
                fileItemButton.style.color = "var(--text-body)"
                fileItemButton.innerHTML = `<i class="${fileIconClass} mr-2"></i>${fileEntry.fileName} <span class="is-size-8 has-text-grey">(${fileEntry.fileCategory || "Source"})</span>`

                // Add a click event listener to load and display the selected source file.
                fileItemButton.addEventListener("click", async () => {
                    // Remove active styling from all file items in the current repository tree.
                    document.querySelectorAll(`.${repositoryConfiguration.itemClass}`).forEach(element => {
                        element.classList.remove("is-active-file")
                        element.style.background = "transparent"
                        element.style.color = "var(--text-body)"
                    })

                    // Apply active file styling to the clicked item button.
                    fileItemButton.classList.add("is-active-file")
                    fileItemButton.style.background = "rgba(16, 124, 16, 0.2)"
                    fileItemButton.style.color = "var(--text-heading)"

                    // Update the active file label display with the selected file name and icon.
                    if (activeFileLabelElement) {
                        activeFileLabelElement.innerHTML = `<i class="${fileIconClass} mr-2"></i> ${fileEntry.fileName}`
                    }

                    // Fetch and render the source code content inside the code container.
                    if (activeCodeContainer) {
                        await fetchAndDisplaySourceFile(fileEntry, activeCodeContainer, repositoryConfiguration.baseDirectory)
                    }

                    // Smoothly scroll the code viewer wrapper into view.
                    const viewerWrapper = document.getElementById(repositoryConfiguration.viewerWrapperId)

                    if (viewerWrapper) {
                        viewerWrapper.scrollIntoView({ behavior: 'smooth', block: 'start' })
                    }
                })

                filesListContainer.appendChild(fileItemButton)
            })

            folderWrapperElement.appendChild(filesListContainer)
            treeContainerElement.appendChild(folderWrapperElement)
        })
    } catch (error) {
        // Display an error message inside the tree container if initialization fails.
        treeContainerElement.innerHTML = `<span class="has-text-danger">// Error loading manifest: ${error.message}</span>`
    }
}

/**
 * Fetches the raw text content of a source file, determines its language class, and triggers syntax highlighting.
 * @param {Object} fileEntry - The metadata entry for the target file.
 * @param {HTMLElement} codeContainerElement - The container element where code is rendered.
 * @param {string} baseDirectory - The base directory path of the repository.
 */
async function fetchAndDisplaySourceFile(fileEntry, codeContainerElement, baseDirectory) {
    // Find and display the parent code viewer wrapper element.
    const parentWrapperElement = codeContainerElement.closest(".code-viewer-wrapper")

    if (parentWrapperElement) {
        parentWrapperElement.style.display = "block"
    }

    // Display a temporary loading message in the code container.
    codeContainerElement.textContent = `// Loading ${fileEntry.fileName}...`
    let languageClass = "language-plaintext"

    // Determine the highlight.js language class based on the file extension.
    if (fileEntry.fileName) {
        const lowercaseFileName = fileEntry.fileName.toLowerCase()

        if (lowercaseFileName.endsWith(".cpp") || lowercaseFileName.endsWith(".hpp") || lowercaseFileName.endsWith(".cc") || lowercaseFileName.endsWith(".cxx")) {
            languageClass = "language-cpp"
        } else if (lowercaseFileName.endsWith(".c") || lowercaseFileName.endsWith(".h")) {
            languageClass = "language-c"
        } else if (lowercaseFileName.endsWith(".cs")) {
            languageClass = "language-csharp"
        } else if (lowercaseFileName.endsWith(".py") || lowercaseFileName.endsWith(".pyx") || lowercaseFileName.endsWith(".pyi") || lowercaseFileName.endsWith(".pxd")) {
            languageClass = "language-python"
        } else if (lowercaseFileName.endsWith(".java")) {
            languageClass = "language-java"
        } else if (lowercaseFileName.endsWith(".go")) {
            languageClass = "language-go"
        }
    }

    try {
        // Throw an error if the file path is missing or undefined.
        if (!fileEntry.filePath) {
            throw new Error("Target file path is undefined or null.")
        }

        // Trim whitespace and format the resolved file path.
        let resolvedFilePath = fileEntry.filePath.trim()

        if (resolvedFilePath.startsWith("/")) {
            resolvedFilePath = resolvedFilePath.slice(1)
        }

        // Construct or clean up the full file path using the base directory.
        if (resolvedFilePath.startsWith("code/darwin-systems/") || resolvedFilePath.startsWith("code/python-mac-os/")) {
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

        // Encode the file path and fetch the file content.
        const encodedFilePath = encodeURI(resolvedFilePath)
        const fileResponse = await fetch(encodedFilePath)

        // Throw an error if the network request fails.
        if (!fileResponse.ok) {
            throw new Error(`HTTP ${fileResponse.status} (${fileResponse.statusText}) attempting to fetch: ${resolvedFilePath}`)
        }

        // Read the file content as text.
        const fileContentText = await fileResponse.text()

        // Inject the file content into the code container and apply the language class.
        codeContainerElement.textContent = fileContentText
        codeContainerElement.removeAttribute("data-highlighted")
        codeContainerElement.className = languageClass

        // Apply highlight.js syntax highlighting if available in the global window object.
        if (window.hljs && typeof window.hljs.highlightElement === "function") {
            window.hljs.highlightElement(codeContainerElement)
        }
    } catch (error) {
        // Log the error and display an error message in the code container.
        console.error("Error fetching file content:", error)
        codeContainerElement.textContent = `// ERROR: Could not load file from path -> ${fileEntry.filePath}\n// Details: ${error.message}`
    }
}

/**
 * Clears and collapses the C repository code viewer state.
 */
function collapseAndClear() { 
    resetRepositoryViewerState("active-code-container", "active-file-label", "c-tree-file-item") 
}

/**
 * Clears and collapses the Python repository code viewer state.
 */
function collapseAndClearPython() { 
    resetRepositoryViewerState("python-active-code-container", "python-active-file-label", "python-tree-file-item") 
}

/**
 * Clears and collapses the Go repository code viewer state.
 */
function collapseAndClearGolang() { 
    resetRepositoryViewerState("golang-active-code-container", "golang-active-file-label", "golang-tree-file-item") 
}

/**
 * Clears and collapses the Under The Hood ML repository code viewer state.
 */
function collapseAndClearUnderTheHood() { 
    resetRepositoryViewerState("under-the-hood-active-code-container", "under-the-hood-active-file-label", "under-the-hood-tree-file-item") 
}

/**
 * Resets the code container, file label, and tree item selection states for a given repository.
 * @param {string} codeContainerId - The ID of the code container element.
 * @param {string} fileLabelId - The ID of the file label element.
 * @param {string} itemClass - The CSS class name used for tree file items.
 */
function resetRepositoryViewerState(codeContainerId, fileLabelId, itemClass) {
    // Retrieve DOM elements for the specified code container and file label.
    const activeCodeContainer = document.getElementById(codeContainerId)
    const activeFileLabelElement = document.getElementById(fileLabelId)

    // Clear the code container content and hide its parent wrapper if present.
    if (activeCodeContainer) {
        activeCodeContainer.textContent = ""
        activeCodeContainer.removeAttribute("data-highlighted")

        const parentWrapperElement = activeCodeContainer.closest(".code-viewer-wrapper")

        if (parentWrapperElement) {
            parentWrapperElement.style.display = "none"
        }
    }

    // Reset the file label text to indicate no file is currently selected.
    if (activeFileLabelElement) {
        activeFileLabelElement.innerHTML = `<i class="fas fa-file-code mr-2"></i> No file selected`
    }

    // Remove active selection styling from all tree file items in the repository.
    document.querySelectorAll(`.${itemClass}`).forEach(element => {
        element.classList.remove("is-active-file")
        element.style.background = "transparent"
        element.style.color = "var(--text-body)"
    })
}