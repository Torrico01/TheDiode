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
// GSAP animations
// --- Slider ---
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

// --- Home Projects ---
gsap.registerPlugin(Flip);
const cards = document.querySelectorAll(".card");
const closeBtns = document.querySelectorAll(".close-btn");
let closeBtnClicked

closeBtns.forEach((btn,index) => {
    btn.addEventListener("click", () => {
        closeBtnClicked = true
    })
})

cards.forEach((card,index) => {
    card.addEventListener("click", () => {
        // Get state
        const state = Flip.getState(cards);
        let arrAbsoluteSelectors = []

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


            Flip.from(state, {
                duration: 1.2,
                ease: "expo.out",
                absolute: true,
                //onComplete: () => {
                //    gsap.to(".card p", {y: 500})
                //}
            })
        }

    });
});



// ------------------