const databaseAddress = "/databaseReq";
var currentID = -1;

function updateID(){
    //uses regex to search for the current recipe id cookie in the webpage cookies
    foundResults = /currecid=(\d+);?/.exec(document.cookie)

    console.log(`all cookies: ${document.cookie}`);

    //checks if some results were found (if not no updating needs to happen)
    if(foundResults != null){

        //reads the value of current id
        currentID = Number(foundResults[1]);

        //overwrites the cookie to so that a new recipe will be generated next time the page opens
        document.cookie = "currecid=-1;path=/";

        console.log(`id: ${currentID})`);

        //checks if the read ID wasn't -1
        if(currentID > 0){
            //updates the page with the current recipe
            readDatabase(currentID);
        }

    }else{

        console.log("no ID found");
    }
}

//updates/posts the current recipe in the editor to the database
function postRecipeUpdate(){
    
    GenerateSQLInsert(ReadEntry(),currentID);
}

function writeEntry(RecipeEntry){

    //writes the entry name
    document.getElementById("name").innerHTML = RecipeEntry.name;

    //writes the entry description
    document.getElementById("desc").innerHTML = RecipeEntry.description;

    //writes the ingredients list
    if(RecipeEntry.ingredients.length == 0){
        //if there are no ingredients, writes an empty entry to the ingredients list
        document.getElementById("ingreds").innerHTML = "<li><br></li>";

    }else{

        //clears the list
        document.getElementById("ingreds").innerHTML = "";

        //adds each ingredient to the list
        for(var i = 0; i < RecipeEntry.ingredients.length; i++){
            document.getElementById("ingreds").innerHTML += "<li>" + RecipeEntry.ingredients[i] + "</li>";
        }
    }

    //writes the instructions list
    if(RecipeEntry.instructions.length == 0){
        //if there are no instructions, writes an empty entry to the instructions list
        document.getElementById("instructs").innerHTML = "<li><br></li>";

    }else{

        //clears the list
        document.getElementById("instructs").innerHTML = "";

        //adds each step to the list
        for(var i = 0; i < RecipeEntry.instructions.length; i++){
            document.getElementById("instructs").innerHTML += "<li>" + RecipeEntry.instructions[i] + "</li>";
        }
    }
}

function GenerateSQLInsert(RecipeEntry, id){
    SQLCommand = "";

    //checks if this recipe has an invalid ID (a new recipe should be created)
    if(id < 0){

        SQLCommand = `INSERT INTO Recipes 
        (name, description, ingredients, instructions)
        VALUES
        ('${RecipeEntry.name}', '${RecipeEntry.description}', '${JSON.stringify(RecipeEntry.ingredients)}', '${JSON.stringify(RecipeEntry.ingredients)}');`;

    }else{
        SQLCommand = `UPDATE Recipes SET 
        Name='${RecipeEntry.name}', 
        description='${RecipeEntry.description}', 
        ingredients='${JSON.stringify(RecipeEntry.ingredients)}', 
        instructions='${JSON.stringify(RecipeEntry.ingredients)}'
        WHERE
        RecipeID = ${id};`;

    }

    //sends sql command to server
    SendQuery(SQLCommand, function(){
        console.log("database updated");
    });

    //returns the ID of the new recipe
    return id;

}

//script for reading data from list
function ReadEntry(){
    //creates a generic recipe entry object
    var RecipeEntry = {
        name: "temp",
        description: "temp",
        ingredients: [],
        instructions: []
    };

    //reads the recipe entry name fromt he html
    RecipeEntry.name = document.getElementById("name").innerHTML;

    //reads the recipe entry description from the html
    RecipeEntry.description = document.getElementById("desc").innerHTML;

    //reads the recipe entry ingredients from the html
    var ingreds = document.getElementById("ingreds")

    for(var i =0; i < ingreds.children.length; i++){
        if(ingreds.children[i].innerHTML != "<br>"){
            //adds the current ingredient to the list
            RecipeEntry.ingredients.push(ingreds.children[i].innerHTML);
        }
    }

    //reads the recipe entry instructions from the html
    var instructs = document.getElementById("instructs");

    for(var i =0; i < instructs.children.length; i++){
        if(instructs.children[i].innerHTML != "<br>"){
            //adds the current instruction to the list
            RecipeEntry.instructions.push(instructs.children[i].innerHTML);
        }
    }

    return RecipeEntry;

}

function SendQuery(SQL_Query, successHandler){

    $.ajax({
        type: "POST",
        url: databaseAddress,
        data: SQL_Query,
        contentType: "application/json",
        dataType: "json",
        success: successHandler,
        error: function (result, status, errThrown) {
            console.log("something went wrong!");
            console.log(`status was ${status}`);
            console.log(`results were ${result.text}`);
            console.log(`threw: ${errThrown}`);
        }
    });
}

function readDatabase(id){

    //sends a query to the database for the recipe at the specifed index
    SendQuery(
        `SELECT name, description, ingredients, instructions FROM recipes WHERE recipeid = ${id};`,
        function(content){
            //interprest the Database results into a recipe entry
            var RecipeEntry = {
                name: content.results[0][0],
                description: content.results[0][1],
                ingredients: content.results[0][2],
                instructions: content.results[0][3]
            };

            //updates the page to the current recipe entry
            writeEntry(RecipeEntry);
        }
    );
}