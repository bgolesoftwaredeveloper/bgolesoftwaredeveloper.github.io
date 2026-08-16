document.addEventListener('DOMContentLoaded', () => {
    const targetNodes = document.querySelectorAll('.scroll-reveal')

    const viewObserver = new IntersectionObserver((entries, observer) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.classList.add('is-visible');
                observer.unobserve(entry.target);
            }
        })
    }, {
        root: null,
        rootMargin: '0px 0px -100px 0px',
        threshold: 0.15
    })

    targetNodes.forEach(node => viewObserver.observe(node))
});