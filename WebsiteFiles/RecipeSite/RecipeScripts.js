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
        RecipeHTML = RecipeHTML.concat(`<h3>${result.name}</h3>`);

        //starts the ingredient list
        RecipeHTML = RecipeHTML.concat(`<ul>`);

        //loops for each ingredient
        for (var j = 0; j < result.ingredients.length; j++){
            RecipeHTML = RecipeHTML.concat(`<li>${result.ingredients[j]}</li>`);
        }

        //ends the ingredients list
        RecipeHTML = RecipeHTML.concat(`</ul>`);

        //starts the instructions list
        RecipeHTML = RecipeHTML.concat(`<ol>`);

        //loops for each ingredient
        for (var j = 0; j < result.instructions.length; j++){
            RecipeHTML = RecipeHTML.concat(`<li>${result.instructions[j]}</li>`);
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
    Ingreds = [docParts[1].textContent];
    Instructs = [docParts[2].textContent];

    //sends the request
    PostRecipe(name, Ingreds, Instructs);
}