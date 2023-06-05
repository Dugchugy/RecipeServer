# RecipeServer
A server to store a variety of recipe's for easy access

# Request Structure

# Recipe Requests

recipe search request
{
    "keywords": list[string]
    "tags": list[string]
}

recipe results response
{
    "SearchResults": list[recipe]
}

recipe json:
{
    "name": string
    "ingredients": list[string]
    "instructions": list[string]
    "ID": int
}