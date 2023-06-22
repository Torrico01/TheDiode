gsap.registerPlugin(Flip); // Flip between css classes with animation

// Cookie functions
function eraseCookie(name) {   
    document.cookie = name +'=; Path=/; Expires=Thu, 01 Jan 1970 00:00:01 GMT;';
}
function getCookie(name) {
    var nameEQ = name + "=";
    var ca = document.cookie.split(';');
    for(var i=0;i < ca.length;i++) {
        var c = ca[i];
        while (c.charAt(0)==' ') c = c.substring(1,c.length);
        if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length,c.length);
    }
    return null;
}
function setCookie(name,value,days) {
    var expires = "";
    if (days) {
        var date = new Date();
        date.setTime(date.getTime() + (days*24*60*60*1000));
        expires = "; expires=" + date.toUTCString();
    }
    document.cookie = name + "=" + (value || "")  + expires + "; path=/";
}
function setCookieAndRedirect(name,value,days,url) {
    setCookie(name,value,days);
    document.location=url;
}
function appendCookieAndRedirect(name,value,days,separator,url) {
    var cookie = getCookie(name);
    if (cookie) {
        cookie = cookie + separator + value;
        setCookieAndRedirect(name,cookie,days,url);
    }
    else {
        setCookieAndRedirect(name,value,days,url);
    }
}
// ------------------
// Change page delay
function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}
function changePage(url) {
    sleep(750).then(() => {
    document.location=url; });
}
// ------------------
// ============ GSAP animations ============
// ------ Home pannel interface ------
const draggables = document.querySelectorAll('.interface-item')
const containers = document.querySelectorAll('.interface-grid')
let baloonIsOpen = false
let currentRow = 1
let currentCol = 1

draggables.forEach(draggable => {
    draggable.addEventListener('dragstart', () => {
        let fullStyle = getComputedStyle(draggable)
        currentRow = parseInt(fullStyle["gridRowStart"])
        currentCol = parseInt(fullStyle["gridColumnStart"])
        draggable.classList.add('dragging')
    })

    draggable.addEventListener('dragend', e => {
        //draggable.classList.remove('dragging')
    })

    draggable.addEventListener('click', () => {
        if (!baloonIsOpen) {
            let baloonActive = 0
            let projectClicked = draggable.id
            let draggableList = document.getElementsByClassName("item-menu-baloon")

            // Find baloon of clicked project
            for (let drag of draggableList) {
                if (drag.id === projectClicked) { baloonActive = drag }
            }

            const state = Flip.getState(baloonActive, {props: "opacity,transform"});
            baloonActive.classList.add("active");
            Flip.from(state, {
                duration: .55,
                absolute: baloonActive.children,
                ease: "elastic.out(0.6, 0.9)",
                scale: true,
                onStart: () => { baloonIsOpen = true }
            })
        }
    })
})

containers.forEach(container => {
    container.addEventListener('click', () => {
        if (baloonIsOpen) {
            let baloonToClose = document.getElementsByClassName('item-menu-baloon active')[0];

            const state = Flip.getState(baloonToClose, {props: "opacity,transform"});
            baloonToClose.classList.remove("active");
            Flip.from(state, {
                duration: 0.55,
                absolute: baloonToClose.children,
                ease: "elastic.out(0.6, 0.9)",
                scale: true,
                onStart: () => { baloonIsOpen = false }
            })
        }
    }); 

    container.addEventListener('dragend', e => {
        e.preventDefault()
        const draggable = document.querySelector('.dragging');
        let projectClicked = draggable.id

        // Find baloon to update row and col
        let baloonToUpdate = 0
        let baloonList = document.getElementsByClassName("item-menu-baloon")
        for (let drag of baloonList) {
            if (drag.id === projectClicked) { baloonToUpdate = drag }
        }

        let offset = e.target.getBoundingClientRect();
        let X = e.clientX - offset.left;
        let Y = e.clientY - offset.top;

        let newRow = currentRow + Math.ceil(Y / 50) - 1
        let newCol = currentCol + Math.ceil(X / 50) - 1
        if (newRow < 1) newRow = 1
        if (newRow > 10) newRow = 10
        if (newCol < 1) newCol = 1
        if (newCol > 22) newCol = 22
        
        draggable.classList.remove('dragging')
        draggable.style.gridRow = newRow
        draggable.style.gridColumn = newCol
        baloonToUpdate.style.gridRow = newRow
        baloonToUpdate.style.gridColumn = newCol        
    })
})

// ------ Home Card Projects ------
const cards = document.querySelectorAll(".card");
const closeBtns = document.querySelectorAll(".close-btn");
let closeBtnClicked = false
let animatingCards = false;

closeBtns.forEach((btn,index) => {
    btn.addEventListener("click", () => {
        closeBtnClicked = true
    })
})

cards.forEach((card,index) => {
    card.addEventListener("click", () => {
        // Step 1: Get current state of all cards 
        const state = Flip.getState(cards);

        // Step 2: Modify the cards somehow. In this case, change css class
        // Add the active class to the clicked one and add inactive to others
        const isCardActive = card.classList.contains("active");

        if (closeBtnClicked || !isCardActive) {
            closeBtnClicked = false

            cards.forEach((otherCard, otherIdx) => {
                otherCard.classList.remove("active");
                otherCard.classList.remove("is-inactive");
                if(!isCardActive && index !== otherIdx) {
                    otherCard.classList.add("is-inactive");
                }
            });
            if(!isCardActive) card.classList.add("active");

            // Step 3: make the transition from state to current modification
            Flip.from(state, {
                duration: 0.7,
                ease: "expo.out",
                absolute: true,
                onStart: () => {
                    card.classList.remove("expanded")
                },
                onComplete: () => { animatingCards = false }
            })
            animatingCards = true

            if(!isCardActive) window.scrollTo(0, 1200);
        }

    });
});

cards.forEach((card,index) => {
    card.addEventListener("mouseenter", () => {
        if (!animatingCards && !card.classList.contains("active")) {
            const state = Flip.getState(card, {props: "border,background"});

            card.classList.add("expanded");

            Flip.from(state, {
                duration: 1,
                ease: "expo.out",
            })
        }
    });
});

cards.forEach((card,index) => {
    card.addEventListener("mouseleave", () => {
        if (!animatingCards) {
            const state = Flip.getState(card, {props: "border,background"});

            card.classList.remove("expanded");

            Flip.from(state, {
                duration: 1,
                ease: "expo.out",
            })
        }
    }); 
});

// ------ Slider when changing pages ------
const timeline1 = gsap.timeline({ defaults: { duration: 0.8 } })
timeline1
    .to('.fromLeft', { duration: 0, opacity: '100%'})
    .from('.fromLeft', { x: '-100%', ease: 'elastic.out(0.6, 0.9)' }) // Elements that slide from left

const timeline2 = gsap.timeline({ defaults: { duration: 0.8 } })
timeline2
    .to('.fromRight', { duration: 0, opacity: '100%'})
    .from('.fromRight', { x: '100%', ease: 'elastic.out(0.6, 0.9)' }) // Elements that slide from right

//gsap.to('body', { overflow: 'auto', delay: 0.8 });

// Happens after button click
const allClickEffectBtns = document.querySelectorAll('.btnWithSliderEffect') // Buttons that trigger effects on click
const submitEffectBtn = document.getElementById('form-create') // Create form that trigger effects on submit

allClickEffectBtns.forEach((clickEffectBtn) => {
    clickEffectBtn.addEventListener('click', () => {
        // Timeline for slider animation of home page
        timeline1.duration(0.8)
        timeline1.reverse()
        // Timelinefor slider animation of components pages
        timeline2.duration(0.8)
        if (clickEffectBtn.role == "home") {
            timeline2.to('.fromRight', { x: '100%', ease: 'elastic.in(0.6, 0.9)' }) }
        else {
            timeline2.to('.fromRight', { x: '-100%', ease: 'elastic.in(0.6, 0.9)' }) }
    });
});

if (submitEffectBtn) {
    submitEffectBtn.addEventListener('submit', () => {
        // Timeline for slider animation of home page
        timeline1.duration(0.8)
        timeline1.reverse()
        // Timelinefor slider animation of components pages
        timeline2.duration(0.8)
        if (submitEffectBtn.role == "back") {
            timeline2.to('.fromRight', { x: '100%', ease: 'elastic.in(0.6, 0.9)' }) }
        else {
            timeline2.to('.fromRight', { x: '-100%', ease: 'elastic.in(0.6, 0.9)' }) }
    });
}

// ------------------