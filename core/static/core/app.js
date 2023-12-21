gsap.registerPlugin(Flip); // Flip between css classes with animation
let home_url = ""

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
function reload_element(id){
    var container = document.getElementById(id);
    var content = container.innerHTML;
    container.innerHTML= content; 
}
function reload_page(){
    sleep(100).then(() => {
        location.reload(); });
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
let interface_grid = document.querySelector('.interface-grid')
let connection_canvas = document.getElementById('connectionCanvas')
let interface_container = document.querySelector('.interface-container')
let selectingConnection = false
let baloonIsOpen = false
let currentRow = -1
let currentCol = -1
let project1_name = "" // Connection line
let project1_pk = -1 // Connection line
let project1_model = -1 // Connection line
let project2_name = "" // Connection line
let project2_pk = -1 // Connection line
let project2_model = -1 // Connection line

// Create baloon (of item or line)
function createBaloon(container, gr, gc, data, id_name, type) {
    // Add html element to represent baloon
    // ----------------
    // container      - base html element,
    // gr             - grid row as reference,
    // gc             - grid column as reference,
    // data           - string separated by ',' that contains html data-properties
    // id_name        - base id that identifies the element in grid
    // type           - type of baloon (line or item)
    //-----------------
    // div:
    const newDiv = document.createElement("div")
    newDiv.setAttribute("id", id_name+'-baloon')
    if (type == 'line') {
        newDiv.setAttribute("class", "line-menu-baloon")
        newDiv.setAttribute("style", "grid-row:"+gr+"; grid-column:"+gc+";")
    }
    else if(type == 'item') {
        newDiv.setAttribute("class", "item-menu-baloon")
        newDiv.setAttribute("style", "grid-row:"+gr+"; grid-column:"+gc+";")
    }
    // div:
    const newDiv1 = document.createElement("div")
    newDiv1.setAttribute("class", "triangle-left-border")
    newDiv.appendChild(newDiv1)
    // div:
    const newDiv2 = document.createElement("div")
    newDiv2.setAttribute("class", "triangle-left-white")
    newDiv.appendChild(newDiv2)
    // ul:
    const newUl = document.createElement("ul")
    newUl.setAttribute("id", id_name)
    if (type == 'line') {
        newUl.setAttribute("class", "line-menu")
    }
    else if(type == 'item') {
        newUl.setAttribute("class", "item-menu")
    }
    // li/a:
    const newLi1 = document.createElement("li")
    const newA1 = document.createElement("a")
    newA1.setAttribute("class", "btn-menu page")
    newA1.setAttribute("role", "button")
    if (type == 'line') {
        newA1.innerHTML = "Configure connection"
    }
    else if(type == 'item') {
        newA1.innerHTML = "Project page"
    }
    newLi1.appendChild(newA1)
    newUl.appendChild(newLi1)
    // li/a (create):
    if(type == 'item') {
        const newLi2 = document.createElement("li")
        const newA2 = document.createElement("a")
        newA2.setAttribute("class", "btn-menu create")
        newA2.setAttribute("role", "button")
        newA2.setAttribute("id", id_name)
        newA2.setAttribute("data-model", data.split(",")[0])
        newA2.setAttribute("data-pk", data.split(",")[1])
        newA2.setAttribute("data-name", data.split(",")[2])
        newA2.innerHTML = "Create connection"
        newLi2.appendChild(newA2)
        newUl.appendChild(newLi2)
        newLineCreate([newA2]) // Add function to create button
    }
    // li/a (delete):
    const newLi3 = document.createElement("li")
    const newA3 = document.createElement("a")
    if(type == 'line') {
        newA3.setAttribute("class", "btn-menu line del")
    }
    else if(type == 'item') {
        newA3.setAttribute("class", "btn-menu item del")
    }
    newA3.setAttribute("role", "button")
    newA3.setAttribute("id", id_name)
    if (type == 'line') {
        newA3.setAttribute("data-start_name", data.split(",")[0])
        newA3.setAttribute("data-end_name",  data.split(",")[1])
        newLineDelete([newA3]) // Add function to delete button
    }
    if (type == 'item') {
        newA3.setAttribute("data-model", data.split(",")[0])
        newA3.setAttribute("data-pk", data.split(",")[1])
        newA3.setAttribute("data-name", data.split(",")[2])
        newItemDelete([newA3]) // Add function to delete button
    }
    newA3.innerHTML = "Delete"
    newLi3.appendChild(newA3)
    newUl.appendChild(newLi3)
    //
    newDiv.appendChild(newUl)
    container.append(newDiv)
}

// Create gif html element
function createImgGif(name, model) {
    let newImg
    if (model.includes('ModularStoragePanelBase')) {
        newImg = document.createElement("img")
        newImg.setAttribute("src", "/static/core/imagens/"+model+".gif")
        newImg.setAttribute("style", "width:100%;")
        newImg.setAttribute("data-name", name)
    }
    else {
        newImg = document.createElement("img")
        newImg.setAttribute("src", "/static/core/imagens/ModularStoragePanelBase.gif")
        newImg.setAttribute("style", "width:100%;")
        newImg.setAttribute("data-name", name)
    }
    return newImg
}

// Create item (in interface panel or projects list)
function createItem(container, gr, gc, name, data, id_name, type) {
    // Add html element to represent baloon
    // ----------------
    // interface_grid - base html element,
    // gr             - grid row as reference,
    // gc             - grid column as reference,
    // data           - string separated by ',' that contains html data-properties
    // name           - name of the project
    // id_name        - base id that identifies the element in grid
    // type           - type of item (panel or list)
    //-----------------
    const model = data.split(",")[0]
    const pk = data.split(",")[1]
    // div
    const newDiv = document.createElement("div")
    newDiv.setAttribute("id", id_name+'-item')
    if (type == 'list') {
        newDiv.setAttribute("class", 'interface-item list')
    }
    else {
        newDiv.setAttribute("class", 'interface-item')
    }
    newDiv.setAttribute("data-model", model)
    newDiv.setAttribute("data-pk", pk)
    if (type == 'list') {
        newDiv.setAttribute("data-inlist", "true")
    }
    newDiv.setAttribute("draggable", "true")
    newDiv.setAttribute("style", "grid-row:"+gr+"; grid-column:"+gc+";")
    if (gr != '0' && gv != '0') {
        const newImg = createImgGif(name, model)
        newDiv.append(newImg)
    }
    else {
        newDiv.innerHTML = name
    }
    container.append(newDiv)
}

// Add 'drag effect' and 'open grid view' (dragstart)
//     'close grid view' (dragend)
//     'open item baloon' and 'stop connection line' (click)
// To new item in panel
function newDraggable(draggables) {
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

        draggable.addEventListener('mouseover', () => {
            const state = Flip.getState(draggable, {props: "border-color,transform"})
            draggable.classList.add("mouseover")
            Flip.from(state, {
                duration: 1,
                scale: true,
                ease: "elastic.out(0.6, 0.9)",
            })
        })

        draggable.addEventListener('mouseout', () => {
            const state = Flip.getState(draggable, {props: "border-color,transform"})
            draggable.classList.remove("mouseover")
            Flip.from(state, {
                duration: 1,
                scale: true,
                ease: "elastic.out(0.6, 0.9)",
            })
        })

        // Open item baloon and stop connection
        draggable.addEventListener('click', () => {
            // Open item baloon
            if (!baloonIsOpen && !selectingConnection) {
                // Get clicked item start row and col
                let fullStyle = getComputedStyle(draggable)
                currentRow = parseInt(fullStyle["gridRowStart"])
                currentCol = parseInt(fullStyle["gridColumnStart"])

                // Get baloon of clicked item
                let projectClicked = draggable.id.split("-")[0]
                let baloonActive = document.getElementById(projectClicked+'-baloon')

                // Animate baloon pop-up
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

                // Properties of draggable
                let name = ""
                try { name = draggable.firstElementChild.dataset.name }
                catch { name = draggable.innerHTML.trim() }
                let model = draggable.dataset.model
                let pk = draggable.dataset.pk
                let project2_id = draggable.id.split("-")[0]

                // --------------- Calculate position and draw line with canvas ---------------
                // Init/clear canvas
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
                //drawConnection(canvas,ctx,x_end,y_end)

                // --------------- Draw line with div and style ---------------
                // Calculate middle point between newRow, newCol and currentRow, currentCol
                // Calculate angle between newRow, newCol and currentRow, currentCol
                // Calculate scale between newRow, newCol and currentRow, currentCol
                const dist_ver = Math.abs(newRow-currentRow)
                const dist_hor = Math.abs(newCol-currentCol)
                let position_y = Math.min(y_start,y_end) + 25*dist_ver
                let position_x = Math.min(x_start,x_end) + 25*dist_hor
                let translate_y = (position_y-25).toString()
                let translate_x = (position_x-25).toString()
                translate_y = 25*(newRow-currentRow)
                translate_x = 25*(newCol-currentCol)
                let angle = (Math.atan(dist_ver/dist_hor)).toString()
                if (y_start > y_end) angle = -angle
                if (x_start > x_end) angle = -angle
                const scale = Math.sqrt(Math.pow(dist_ver,2) + Math.pow(dist_hor,2))
                // Try to create line element
                let lineName = project1_id+'-'+project2_id+"-line"
                let lineElement = document.getElementById(lineName)
                if (!lineElement) {
                    // Add html element to represent line
                    const newDiv = document.createElement("div");
                    newDiv.setAttribute("id", lineName)
                    newDiv.setAttribute("class", "line-connection")
                    newDiv.setAttribute("style", "position: relative; background-color: rgb(0, 0, 55); grid-row:"+currentRow+"; grid-column:"+currentCol+"; transform: rotate("+angle+"rad) scale("+scale+", 0.3); left:"+translate_x+"px; top:"+translate_y+"px;")
                    interface_grid.prepend(newDiv)
                    // Get line and add event listener
                    lineElement = document.getElementById(lineName)
                    lineElement.addEventListener('click', () => {
                        // Open line baloon
                        if (!baloonIsOpen && !selectingConnection) {
                        // Get clicked line projects
                        let p1 = lineElement.id.split("-")[0]
                        let p2 = lineElement.id.split("-")[1]
                
                        // Get baloon of clicked line
                        let baloonActive = document.getElementById(p1+'-'+p2+'-baloon')
                
                        // Animate baloon pop-up
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
                }
                else {
                    lineElement.setAttribute("style", "position: relative; background-color: rgb(0, 0, 55); grid-row:"+currentRow+"; grid-column:"+currentCol+"; transform: rotate("+angle+"rad) scale("+scale+", 0.3); left:"+translate_x+"px; top:"+translate_y+"px;")
                }

                // --------------- Add connection to data base ---------------
                project2_name = name
                project2_pk = pk
                project2_model = model
                // Make post request to create
                let xhr = new XMLHttpRequest();
                xhr.open("POST", home_url);
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

                // --------------- Add line balloon ---------------
                let balloonLineName = project1_id+'-'+project2_id
                let balloonLineElement = document.getElementById(balloonLineName+"-baloon")
                if (!balloonLineElement) {
                    let data = project1_name+","+project2_name
                    let gr = Math.floor((currentRow+newRow)/2)
                    let gc = Math.floor((currentCol+newCol)/2)
                    createBaloon(interface_grid, gr, gc, data, balloonLineName, 'line');
                }
            }
        })
    })
}

// Add 'open line baloon' (click)
// To new line
function newConnection(connections) {
    connections.forEach(connection => {
        connection.addEventListener('click', () => {
        // Open line baloon
        if (!baloonIsOpen && !selectingConnection) {
            // Get clicked line projects
            let p1 = connection.id.split("-")[0]
            let p2 = connection.id.split("-")[1]

            // Get baloon of clicked line
            let baloonActive = document.getElementById(p1+'-'+p2+'-baloon')

            // Animate baloon pop-up
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
}

// Add 'close baloon' (click)
//     'draw connection line with mouse' (mousemove)
//     'update position of dragged item' (dragend)
// To interface panel
if(interface_container) {
    // Close baloon
    interface_container.addEventListener('click', () => {
        if (baloonIsOpen) {
            let baloonToClose = -1
            if (document.getElementsByClassName('item-menu-baloon active')[0]) {
                baloonToClose = document.getElementsByClassName('item-menu-baloon active')[0];
            }
            else {
                baloonToClose = document.getElementsByClassName('line-menu-baloon active')[0];
            }

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
            e.preventDefault()
            let offset = interface_grid.getBoundingClientRect();

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

    // Update position of dragged item
    interface_container.addEventListener('dragend', e => {
        e.preventDefault()
        const draggable = document.querySelector('.dragging');
        let projectClicked = draggable.id.split("-")[0]
        let offset = interface_grid.getBoundingClientRect();

        // Properties of draggable
        let name = ""
        try { name = draggable.firstElementChild.dataset.name }
        catch { name = draggable.innerHTML.trim() }
        let model = draggable.dataset.model
        let pk = draggable.dataset.pk

        // Get new row and column
        const x_end = e.clientX - offset.left
        const y_end = e.clientY - offset.top
        let newRow = Math.ceil(y_end / 50)
        let newCol = Math.ceil(x_end / 50)
        if (newRow < 1) newRow = 1
        if (newRow > 10) newRow = 10
        if (newCol < 1) newCol = 1
        if (newCol > 22) newCol = 22
        
        // Update row and column values of item and baloon
        draggable.classList.remove('dragging')
        draggable.style.gridRow = newRow
        draggable.style.gridColumn = newCol

        // Find item baloon to update
        let baloonLineToUpdate = document.getElementById(projectClicked+'-baloon')
        if (!baloonLineToUpdate) {
            let data = model+","+pk+","+name
            createBaloon(interface_grid, newRow, newCol, data, projectClicked, 'item');
            baloonLineToUpdate = document.getElementById(projectClicked+'-baloon')
        }
        baloonLineToUpdate.style.gridRow = newRow
        baloonLineToUpdate.style.gridColumn = newCol
        
        // Update row and column in db
        let xhr = new XMLHttpRequest();
        xhr.open("POST", home_url);
        xhr.setRequestHeader("Accept", "application/json");
        xhr.setRequestHeader("Content-Type", "application/json");
        let data = `{"type": "position",
                    "model": "`+model+`",
                    "name": "`+name+`",
                    "grid_row": `+newRow+`,
                    "grid_col": `+newCol+`}`;
        xhr.send(data);

        // Update connection line when moving item
        // Get all lines and iterate
        let line_connections = document.getElementsByClassName('line-connection')
        for (let line of line_connections) {
            if(line.id.includes(projectClicked)){
                // Get ids of both items
                let lineBaloon = -1
                if(line.id.split("-")[1] === projectClicked) {
                    p1_id = line.id.split("-")[0]
                    p2_id = line.id.split("-")[1]
                    lineBaloon = document.getElementById(p1_id+'-'+p2_id+"-baloon")
                }
                else {
                    p2_id = line.id.split("-")[0]
                    p1_id = line.id.split("-")[1]
                    lineBaloon = document.getElementById(p2_id+'-'+p1_id+"-baloon")
                }

                // Redefine currentRow and currentCol to first item
                p1 = document.getElementById(p1_id+"-item")
                let fullStyle = getComputedStyle(p1)
                currentRow = parseInt(fullStyle["gridRowStart"])
                currentCol = parseInt(fullStyle["gridColumnStart"])

                // Recalculate values
                const x_start = (currentCol*50)-25
                const y_start = (currentRow*50)-25
                const x_end = (newCol*50)-25
                const y_end = (newRow*50)-25
                // Calculate middle point between newRow, newCol and currentRow, currentCol
                // Calculate angle between newRow, newCol and currentRow, currentCol
                // Calculate scale between newRow, newCol and currentRow, currentCol
                const dist_ver = Math.abs(newRow-currentRow)
                const dist_hor = Math.abs(newCol-currentCol)
                let position_y = Math.min(y_start,y_end) + 25*dist_ver
                let position_x = Math.min(x_start,x_end) + 25*dist_hor
                let translate_y = (position_y-25).toString()
                let translate_x = (position_x-25).toString()
                translate_y = 25*(newRow-currentRow)
                translate_x = 25*(newCol-currentCol)
                let angle = (Math.atan(dist_ver/dist_hor)).toString()
                if (y_start > y_end) angle = -angle
                if (x_start > x_end) angle = -angle
                const scale = Math.sqrt(Math.pow(dist_ver,2) + Math.pow(dist_hor,2))

                // Update line style
                line.setAttribute("style", "position: relative; background-color: rgb(0, 0, 55); grid-row:"+currentRow+"; grid-column:"+currentCol+"; transform: rotate("+angle+"rad) scale("+scale+", 0.3); left:"+translate_x+"px; top:"+translate_y+"px;")
                // Update line balloon style
                lineBaloon.style.gridRow = Math.floor((currentRow+newRow)/2)
                lineBaloon.style.gridColumn = Math.floor((currentCol+newCol)/2)
            }
        }

        // Move item from projects list to grid
        if (draggable.dataset.inlist) {
            draggable.setAttribute("class", "interface-item")
            draggable.innerHTML = ''
            draggable.append(createImgGif(name, model))
            interface_grid.prepend(draggable)
        }
        draggable.removeAttribute('data-inlist')
    })
}

// Add 'start creating connection line' (click)
// To 'Create connection' button in item
function newLineCreate(btns_connection) {
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
}
// Add 'delete connection line' (click)
// To 'Delete' button in line
function newLineDelete(btns_line_del) {
    // Delete line from db and html
    btns_line_del.forEach(btn => {
        btn.addEventListener("click", () => {
            baloonIsOpen = false
            let start_name = btn.dataset.start_name
            let end_name = btn.dataset.end_name
            let start_id = btn.id.split("-")[0]
            let end_id = btn.id.split("-")[1]
            // Make post request to delete line
            let xhr = new XMLHttpRequest();
            xhr.open("POST", home_url);
            xhr.setRequestHeader("Accept", "application/json");
            xhr.setRequestHeader("Content-Type", "application/json");
            let data = `{"type": "delete connection",
                        "start_name": "`+start_name+`",
                        "end_name": "`+end_name+`"}`;
            xhr.send(data);
            const lineToDel = document.getElementById(start_id+"-"+end_id+"-line");
            lineToDel.remove();
            const baloonToDel = document.getElementById(start_id+"-"+end_id+"-baloon");
            baloonToDel.remove();
        })
    })
}
// Add 'delete citem' (click)
// To 'Delete' button in item
function newItemDelete(btns_item_del) {
    // Delete items from panel to list
    btns_item_del.forEach(btn => {
        btn.addEventListener("click", () => {
            baloonIsOpen = false
            let model = btn.dataset.model
            let pk = btn.dataset.pk
            let name = btn.dataset.name
            let id = btn.id
            // Make post request to delete line
            let xhr = new XMLHttpRequest();
            xhr.open("POST", home_url);
            xhr.setRequestHeader("Accept", "application/json");
            xhr.setRequestHeader("Content-Type", "application/json");
            let data = `{"type": "delete project",
                        "model": "`+model+`",
                        "name": "`+name+`"}`
            xhr.send(data)

            // Move item from grid to projects list
            projects_list_container = document.getElementById("interface-projects")
            draggable = document.getElementById(id+"-item")
            draggable.setAttribute("class", "interface-item list")
            draggable.setAttribute("data-inlist", "true")
            draggable.setAttribute("style", "grid-row: 0; grid-column: 0;")
            draggable.innerHTML = name
            projects_list_container.appendChild(draggable)

            const baloonToDel = document.getElementById(id+"-baloon");
            baloonToDel.remove();
        })
    })
}

// Canvas functions
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
    ctx.moveTo((currentCol*50)-15, (currentRow*50)-15);

    ctx.strokeStyle = 'rgb(0, 0, 0)';
    ctx.lineWidth = 10;
    ctx.lineTo(x_end, y_end);
    ctx.stroke();

    ctx.strokeStyle = 'rgb(235, 55, 55)';
    ctx.lineWidth = 8;
    ctx.lineTo(x_end, y_end);
    ctx.stroke();
}

// Call initialize functions
newDraggable(document.querySelectorAll('.interface-item'))
newConnection(document.querySelectorAll('.line-connection'))
newLineCreate(document.querySelectorAll('.btn-menu.create'))
newLineDelete(document.querySelectorAll('.btn-menu.line.del'))
newItemDelete(document.querySelectorAll('.btn-menu.item.del'))

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

            if(!isCardActive) window.scrollTo(0, 1500);
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

const modal = new bootstrap.Modal(document.getElementById("modal"))

htmx.on("htmx:beforeSwap", (e) => {
    // Empty response targeting #dialog => hide the modal
    if (e.detail.target.id == "dialog" && !e.detail.xhr.response) {
      modal.hide()
      e.detail.shouldSwap = false
    }
  })

htmx.on("htmx:afterSwap", (e) => {
    // Response targeting #dialog => show the modal
    if (e.detail.target.id == "dialog") {
        modal.show()
    }
})

htmx.on("hidden.modal", () => {
    document.getElementById("dialog").innerHTML = ""
})