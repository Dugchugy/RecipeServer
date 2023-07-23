const recipeSearchIP = "/RecipeSearch";
const recipePostIP = "/RecipePost";

/*
##################################################################
Functions for keyword/tag recipe Search
##################################################################
*/

function QueryRecipes(keywords, tags){

    $.ajax({
        type: "POST",
        url: recipeSearchIP,
        data: JSON.stringify({ "keywords": keywords, "tags": tags}),
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

    console.log(`text result ${content}`);
    
    console.log(`json result: ${JSON.stringify(content)}`);

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
    }

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
}

/*
##################################################
Functions for manipulating the recipe editor page
##################################################
*/

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

        curIndex = Number(DocEntry.children[1].innerHTML)

        //checks if this element is bound to an index in the instructions list
        if(curIndex < 0){
            //if not

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
};