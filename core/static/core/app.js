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
// ------ Home panel interface ------
let draggables = document.querySelectorAll('.interface-item')
let interface_grid = document.querySelector('.interface-grid')
let connection_canvas = document.getElementById('connectionCanvas')
let interface_container = document.querySelector('.interface-container')
let btns_connection = document.querySelectorAll('.btn-item-menu.create')
let selectingConnection = false
let baloonIsOpen = false
let currentRow = 1
let currentCol = 1
let project1_name = "" // Connection line
let project1_pk = 1 // Connection line
let project1_model = 1 // Connection line
let project2_name = "" // Connection line
let project2_pk = 1 // Connection line
let project2_model = 1 // Connection line

// Drag items, open/close grid view, open item baloon and stop connection line
draggables.forEach(draggable => {
    // Start dragging and open grid view
    draggable.addEventListener('dragstart', () => {
        // Get current row and col
        let fullStyle = getComputedStyle(draggable)
        currentRow = parseInt(fullStyle["gridRowStart"])
        currentCol = parseInt(fullStyle["gridColumnStart"])
        draggable.classList.add('dragging')

        // Open grid view
        const interface_grid = document.querySelector(".interface-grid-container")
        const state = Flip.getState(interface_grid, {props: "background"})
        interface_grid.classList.add("active")
        Flip.from(state, {
            duration: 2,
            ease: "expo.out"
        })
    })

    // Close grid view
    draggable.addEventListener('dragend', () => {
        const interface_grid = document.querySelector(".interface-grid-container")
        const state = Flip.getState(interface_grid, {props: "background"})
        interface_grid.classList.remove("active")
        Flip.from(state, {
            duration: 0.3,
            ease: "expo.out"
        })
    })

    // Open baloon and stop connection
    draggable.addEventListener('click', () => {
        // Open baloon
        if (!baloonIsOpen && !selectingConnection) {
            // Get current row and col
            let fullStyle = getComputedStyle(draggable)
            currentRow = parseInt(fullStyle["gridRowStart"])
            currentCol = parseInt(fullStyle["gridColumnStart"])

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
        // Finish connection
        else if (selectingConnection) {
            selectingConnection = false

            // --------------- Calculate position and draw line with canvas ---------------
            // Init canvas
            const [canvas, ctx] = clearAndLoadCanvas()
            // Get new clicked row and col
            let fullStyle = getComputedStyle(draggable)
            const newRow = parseInt(fullStyle["gridRowStart"])
            const newCol = parseInt(fullStyle["gridColumnStart"])
            // Calculate end x and y
            const x_start = (currentCol*50)-25
            const y_start = (currentRow*50)-25
            const x_end = (newCol*50)-25
            const y_end = (newRow*50)-25
            // Draw line
            drawConnection(canvas,ctx,x_end,y_end)

            // --------------- Draw line with div and style ---------------
            let lineName = project1_id+'-'+draggable.id
            let lineElement = document.getElementById(lineName)
            if (!lineElement) {
                // Add html element to represent line
                const newDiv = document.createElement("div");
                newDiv.setAttribute("id", lineName)
                interface_grid.prepend(newDiv)
            }
            // Calculate middle point between newRow, newCol and currentRow, currentCol
            // Calculate angle between newRow, newCol and currentRow, currentCol
            // Calculate scale between newRow, newCol and currentRow, currentCol
            const dist_ver = Math.abs(newRow-currentRow)
            const dist_hor = Math.abs(newCol-currentCol)
            let position_y = Math.min(y_start,y_end) + 25*dist_ver
            let position_x = Math.min(x_start,x_end) + 25*dist_hor
            let translate_y = (position_y-25).toString()
            let translate_x = (position_x-25).toString()
            let angle = (Math.atan(dist_ver/dist_hor)).toString()
            if (y_start > y_end) angle = -angle
            if (x_start > x_end) angle = -angle
            const scale = Math.sqrt(Math.pow(dist_ver,2) + Math.pow(dist_hor,2))
            // To add style attributes
            var style = document.createElement('style');
            style.type = 'text/css'
            style.innerHTML = '.cssClass { position: relative; background-color: rgb(0, 0, 55); transform: rotate('+angle+'rad) scale('+scale+', 0.3); left: '+translate_x+'px; top:'+translate_y+'px; }';
            document.getElementsByTagName('head')[0].appendChild(style);
            document.getElementById(lineName).className = 'cssClass';

            // --------------- Add connection to data base ---------------
            project2_name = draggable.innerHTML.trim()
            project2_pk = draggable.dataset.pk
            project2_model = draggable.dataset.model
            // Make post request to create
            let xhr = new XMLHttpRequest();
            xhr.open("POST", "http://192.168.56.1:8080/");
            xhr.setRequestHeader("Accept", "application/json");
            xhr.setRequestHeader("Content-Type", "application/json");
            let data = `{"type": "connections",
                         "start_name": "`+project1_name+`",
                         "start_type":"`+project1_model+`",
                         "start_obj_id":"`+project1_pk+`",
                         "end_name": "`+project2_name+`",
                         "end_type":"`+project2_model+`",
                         "end_obj_id":"`+project2_pk+`"}`;
            xhr.send(data);
        }
    })
})

// Close baloon and draw connection line
if(interface_container) {
    // Close baloon
    interface_container.addEventListener('click', () => {
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
    })

    // Draw connection line
    interface_container.addEventListener('mousemove', e => {
        if(selectingConnection) {
            // Init canvas
            const [canvas, ctx] = clearAndLoadCanvas()
            ctx.isPointInPath
            // Get mouse postion
            var rect = canvas.getBoundingClientRect();
            const x_end = e.clientX - rect.left
            const y_end = e.clientY - rect.top
            // Draw line from block to mouse
            drawConnection(canvas,ctx,x_end,y_end)
        }
    })
}

// Update position of dragged item
if (interface_grid) {
    interface_grid.addEventListener('dragend', e => {
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
        
        let xhr = new XMLHttpRequest();
        xhr.open("POST", "http://192.168.56.1:8080/");
        xhr.setRequestHeader("Accept", "application/json");
        xhr.setRequestHeader("Content-Type", "application/json");
        let data = `{"type": "position", "model":"`+draggable.dataset.model+`", "name": "`+draggable.innerHTML.trim()+`", "grid_row": `+newRow+`, "grid_col": `+newCol+`}`;
        xhr.send(data);
    })
}

// Start drawing the connection line
btns_connection.forEach(btn => {
    btn.addEventListener("click", () => {
        selectingConnection = true
        project1_id = btn.id
        project1_name = btn.dataset.name
        project1_pk = btn.dataset.pk
        project1_model = btn.dataset.model
    })
})

function clearAndLoadCanvas() {
    // Start canvas
    const canvas = document.getElementById('connectionCanvas')
    if (!canvas.getContext) { return; }
    const ctx = canvas.getContext('2d');

    // Clear Canvas
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Draw other canvas lines
    // ...

    return [canvas,ctx]
}

function drawConnection(canvas,ctx,x_end,y_end) {
    ctx.beginPath();
    ctx.moveTo((currentCol*50)-25, (currentRow*50)-25);

    ctx.strokeStyle = 'rgb(0, 0, 0)';
    ctx.lineWidth = 10;
    ctx.lineTo(x_end, y_end);
    ctx.stroke();

    ctx.strokeStyle = 'rgb(235, 55, 55)';
    ctx.lineWidth = 8;
    ctx.lineTo(x_end, y_end);
    ctx.stroke();
}

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