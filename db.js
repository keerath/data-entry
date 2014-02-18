$(document).ready(function() {
    $.getJSON("/get_data", function(data) {
       $.each(data[0],function(key,value){
        $("#headContainer").append("<th>" + key + "</th>");

     $("#userInput").append("<h5 align = 'left'>&nbsp;Enter " + key + ":&nbsp;" 
                        + "<input class = 'form-control up'  align='left' type='text'\
                        placeholder='" + key + "'/></h5>");
       });

        for (var i in data) {
            $("#table").append("<tr>  </tr>");
            $.each(data[i], function(key, value) {
                $("tr:last").append("<td>" + value + "</td>");
               
            });
        }
    });
    $("#info").append("<button type = 'button' title = 'Add Records'\
                     class = 'glyphicon glyphicon-plus-sign btn btn-info btn-sm highlight '\
                     id='update'>&nbsp;Add</buttton> ");

    
$("#updatedb").click(function(event){
        var send_data = [];
          event.preventDefault();
        var array =[];
         $("tr").not("tr:first").addClass("json");
        $("#table th").each(function(){
            array.push($(this).text().trim());
            
        });
        $("tr.json").each(function(){  
        var index = {};          
        for(var i in array)
        {    var temp1 = array[i].toString();
             var temp2 = $(this).children(":eq("+i+")").text().trim();
             index[temp1]=temp2;
        }
        send_data.push(index);

   });
      
   $.ajax
   ({
    type:"POST",
    async: false,
    url:"/get_data",
    data: encodeURI("tabledata=" + JSON.stringify(send_data)),
    dataType:"json",
   })

});
});