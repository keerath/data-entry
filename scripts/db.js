$(document).ready(function() {
    $.getJSON("/get_data", function(data) {
        var i=0 ;
       $.each(data[0],function(key,value){
        $("#headContainer").append("<th>" + key + "</th>");

     $("#userInput").append("<h5 align = 'left'>&nbsp;Enter " + key + ":&nbsp;" 
                        + "<input class = 'form-control up' name = "+ i +"  align='left' type='text'\
                        placeholder='" + key + "'/></h5>");
        i++;
       });

        for (var i in data) {
            $("#table").append("<tr>  </tr>");
            $.each(data[i], function(key, value) {
                $("tr:last").append("<td>" + value + "</td>");
               
            });
        }
    });
    $("#info").append("<button type = 'button' title = 'Add Records'\
                     class = 'glyphicon glyphicon-plus-sign btn btn-big btn-info highlight '\
                     id='update'>&nbsp;Add</buttton> ");

});