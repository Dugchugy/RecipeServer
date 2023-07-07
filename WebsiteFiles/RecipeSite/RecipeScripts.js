const recipeIP = "/RecipeSearch";

function QueryRecipes(keywords, tags){

    $.ajax({
        type: "POST",
        url: recipeIP,
        data: JSON.stringify({ "keywords": keywords, "tags": tags}),
        contentType: "application/json",
        success: handleRecipeResponse,
        error: function (result, status) {
            console.log("something went wrong!");
            console.log(`status was ${status}`);
            console.log(`results were ${result}`);
        }
    });

}

function handleRecipeResponse(content){
    
    console.log(`json result: ${JSON.stringify(content)}`);

    result = content.SearchResults

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

    console.log(`Searching for ${keywords} with tags ${tags} at url ${recipeIP}`);

    QueryRecipes(keywords, tags);
}