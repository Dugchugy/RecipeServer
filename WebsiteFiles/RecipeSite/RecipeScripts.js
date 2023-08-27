const recipeSearchIP = "/databaseReq";

/*
##################################################################
Functions for keyword/tag recipe Search
##################################################################
*/

function QueryRecipes(keywords, tags){

    $.ajax({
        type: "POST",
        url: recipeSearchIP,
        data: "SELECT name, description, ingredients, instructions, recipeid FROM recipes;",
        contentType: "application/json",
        dataType: "json",
        success: handleRecipeResponse,
        error: function (result, status, errThrown) {
            console.log("something went wrong!");
            console.log(`status was ${status}`);
            console.log(`results were ${result.text}`);
            console.log(`threw: ${errThrown}`);
        }
    });

}

function handleRecipeResponse(content){

    //gets the main div
    mainDiv = document.getElementsByClassName("RecipeHolder")[0];

    //clears the main div
    mainDiv.innerHTML = '';

    var result = content.Results

    console.log(JSON.stringify(result))

    for (var i = 0; i < result.length; i++){

        //starts the recipeDiv
        var RecipeHTML = `<div class="SiteSection">`;

        //adds the recipe title
        RecipeHTML = RecipeHTML.concat(`<h3>${result[i][0]}</h3>`);

        //adds the recipe description
        RecipeHTML = RecipeHTML.concat(`<p>${result[i][1]}</p>`);

        //adds the ingredients

        //starts the ingredient list
        RecipeHTML = RecipeHTML.concat(`<ul>`);

        //loops for each ingredient
        for (var j = 0; j < result[i][2].length; j++){
            RecipeHTML = RecipeHTML.concat(`<li>${result[i][2][j]}</li>`);
        }

        //ends the ingredients list
        RecipeHTML = RecipeHTML.concat(`</ul>`);

        //starts the instructions list

        //starts the instructions list
        RecipeHTML = RecipeHTML.concat(`<ol>`);

        //loops for each ingredient
        for (var j = 0; j < result[i][3].length; j++){
            RecipeHTML = RecipeHTML.concat(`<li>${result[i][3][j]}</li>`);
        }

        //ends the instructions list
        RecipeHTML = RecipeHTML.concat(`</ol>`);

        //adds an open editor button
        RecipeHTML = RecipeHTML.concat(`<a onclick="OpenEditer(${result[i][4]});" href=/RecipeSite/RecipeEditor.htm>Edit Recipe</a>`)

        //ends the recipe div
        RecipeHTML = RecipeHTML.concat(`</div>`);
        mainDiv.innerHTML = mainDiv.innerHTML.concat(RecipeHTML);

    }

        
    /*console.log(`json result: ${JSON.stringify(content)}`);

    result = content.SearchResults

    console.log(`json array results: ${JSON.stringify(result)}`);

    mainDiv = document.getElementsByClassName("RecipeHolder")[0];

    console.log(`elements found ${JSON.stringify(mainDiv)}`)
    
    mainDiv.innerHTML = '';

    for (var i = 0; i < result.length; i++){
        //starts the recipeDiv
        var RecipeHTML = `<div class="SiteSection">`;

        //adds the recipe title
        RecipeHTML = RecipeHTML.concat(`<h3>${result[i].title}</h3>`);

        //starts the ingredient list
        RecipeHTML = RecipeHTML.concat(`<ul>`);

        //loops for each ingredient
        for (var j = 0; j < result[i].ingredients.length; j++){
            RecipeHTML = RecipeHTML.concat(`<li>${result[i].ingredients[j]}</li>`);
        }

        //ends the ingredients list
        RecipeHTML = RecipeHTML.concat(`</ul>`);

        //starts the instructions list
        RecipeHTML = RecipeHTML.concat(`<ol>`);

        //loops for each ingredient
        for (var j = 0; j < result[i].instructions.length; j++){
            RecipeHTML = RecipeHTML.concat(`<li>${result[i].instructions[j]}</li>`);
        }

        //ends the instructions list
        RecipeHTML = RecipeHTML.concat(`</ol>`);

        //ends the recipe div
        RecipeHTML = RecipeHTML.concat(`</div>`);
        mainDiv.innerHTML = mainDiv.innerHTML.concat(RecipeHTML);
    }*/

}

function OpenEditer(id){
    document.cookie = `currecid=${id};path=/`;

    console.log("cookie added");
}

function startRecipeQuery(){

    var keywords = []
    var tags = []

    console.log(`Searching for ${keywords} with tags ${tags} at url ${recipeSearchIP}`);

    QueryRecipes(keywords, tags);
}

/*
##################################################
Functions for posting a Recipe
##################################################
*/

/*
function PostRecipe(title, ingredients, steps){

    $.ajax({
        type: "POST",
        url: recipePostIP,
        data: JSON.stringify({ "title": title, "ingredients": ingredients, "instructions": steps}),
        contentType: "application/json",
        success: function (content){
            console.log("recipe Post successful");
            console.log(`returned ${JSON.stringify(content)}`);
        },
        error: function (result, status) {
            console.log("something went wrong!");
            console.log(`status was ${status}`);
            console.log(`results were ${result}`);
        }
    });

}

function StartRecipePost(){
    //defines varaibles
    var name = "";
    var Ingreds = [];
    var Instructs = [];

    //reads the required components from the document
    var docParts = document.getElementsByClassName("ReadableContent");

    //stores the components
    name = docParts[0].textContent;
    Instructs = [docParts[2].textContent];

    //reads the ingredients list from the html
    Ingreds = JSON.parse(document.getElementById("ingredsList").textContent);

    //sends the request
    PostRecipe(name, Ingreds, Instructs);
}*/

/*
##################################################
Functions for manipulating the recipe editor page
##################################################
*/
/*
function addIngredient(){

    //reads the ingredient to add
    var ingredToAdd = document.getElementsByClassName("ReadableContent")[1].textContent;

    //reads the current ingredient list into a json array
    var ingredList = JSON.parse(document.getElementById("ingredsList").textContent);

    //adds the new ingredient to the list
    ingredList.push(ingredToAdd);

    //re-writes the ingredients list
    document.getElementById("ingredsList").textContent = JSON.stringify(ingredList);

    //gets the element for the ingredients list
    htmlIngredList = document.getElementById("visibleIngredients");

    console.log (`initial html ${htmlIngredList.innerHTML}`);

    //defines an empty string
    rawOut = "";

    //loops for all the ingerdients
    for (var i = 0; i < ingredList.length; i++){
        console.log (`adding ${ingredList[0]} to list`);
        //adds them to the inner html
        rawOut = rawOut.concat(`<li>${ingredList[i]}</li>`);
    }

    //writes the new inner html
    htmlIngredList.innerHTML = rawOut;

    console.log (`final html ${htmlIngredList.innerHTML}`);



}

//event handler to add item to the instructions list
function handleKeyPress(event){
    //only acts on a press of the enter key
    if(event.key == "Enter"){
        // Cancel the default action, if needed
        event.preventDefault();

        //reads the json list for the instructions and stores it
        var InstructList = JSON.parse(document.getElementById("InstructListSoruce").innerHTML);

        //gets the parent element of the element being edited
        var DocEntry = document.activeElement.parentElement;

        var curIndex = Number(DocEntry.children[1].innerHTML)

        //checks if this element is bound to an index in the instructions list
        if(curIndex < 0){
            //if not

            //checks if no text was added to the entry box
            if(document.activeElement.innerHTML == "" || document.activeElement.innerHTML == "Add"){
                //sets the inner text to "Add" in case it's blank
                document.activeElement.innerHTML = "Add";
                
                //since no text was edited, theres nothing to add to the list, returns
                return
            }

            //stores the inedx of the new element that will be added tot the list
            DocEntry.children[1].innerHTML = InstructList.length;

            //also rewrites the curIndex value
            curIndex = InstructList.length;

            //adds a new element to the stored list
            InstructList.push(document.activeElement.innerHTML);

            //adds a new element to the html
            DocEntry.insertAdjacentHTML("afterend", 
            '<div><li contenteditable="true" class="ReadableContent">Add</li><p class="Hidden">-1</p></div>');

        }else{
            //if so

            //checks if the text has been deleted
            if(document.activeElement.innerHTML == ""){
                //loops through all the elements from this one to the end of the list
                for(var i = curIndex; i < InstructList.length - 1; i++){

                    if(i >= InstructList.length - 1){
                        break;
                    }

                    //replaces the current instruction list value with the value of the 
                    InstructList[i] = InstructList[i + 1];

                    //replaces the value in the UI elements
                    DocEntry.parentElement.children[i].children[0].innerHTML = DocEntry.parentElement.children[i + 1].children[0].innerHTML;

                }
                
                //remvoes the last counted item from the html (does not count the Add item)
                DocEntry.parentElement.children[InstructList.length - 1].remove();

                //removes the last item from the array
                InstructList.pop()

                //stores the new list
                document.getElementById("InstructListSoruce").innerHTML = JSON.stringify(InstructList);

                //stops the rest of the function (prevents errors)
                return;
            }

            //changes that element to now have the new value
            InstructList[curIndex] = document.activeElement.innerHTML;
        }

        //saves the updated list
        document.getElementById("InstructListSoruce").innerHTML = JSON.stringify(InstructList);

        console.log(DocEntry.parentElement.children);

        console.log(DocEntry.parentElement.children[0].children);

        //selects the next item in the list
        DocEntry.parentElement.children[curIndex + 1].children[0].focus();
    }
}

window.onload = function(){
    //adds the keypress listener to the Instructions list
    document.getElementById("InstructionsList").addEventListener("keydown", handleKeyPress); 
};*/