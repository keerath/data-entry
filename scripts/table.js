/* global document,alert,$ */
$(document).ready(function () {
    "use strict";
    var send =[];
    var row_indices =[];
    var userInput = $("#userInput"), position;
    $("#table").on('dblclick', 'td', function (event) {
        var flag =1;
        var field;
        event.preventDefault();
        var elem, defaultText;
        defaultText = $(this).text().trim();
        elem = $(this);
        elem.html('<input id= "in" type= "text" value="' + defaultText + '" />');
        elem.children().first().focus();

        $(document).on({
            keypress: function (event) {
                if (event.which === 13&&flag===1) {
                    elem.text(elem.children().val());
                    data_update();
                    flag=0;
                }
            },
            click: function (event) {
                if (event.target.tagName !== "INPUT"&&flag===1) {
                    elem.text(elem.children().val());
                    data_update();
                    flag=0;
                }
            }
        });
        function data_update()
        {   
            var clickpos = $("td").index(elem);
            var rownum = $("tr").index(elem.parent());
            var n = $("th").length;
            if(clickpos>length-1)
            {
                 field = $("th").get(clickpos-n).innerText;
            }
            else
            {
                 field =$("th").get(clickpos).innerText;
            }
         $.ajax
          ({
            type:"POST",
            async:false,
            url:"/update_data",
            data: encodeURI("celldata="+elem.text()+"&rowNum="+rownum+"&field="+field),
            dataType:"text",
            success: function(response)
            {
                alert(response);
            }
          });

        }
    });

    $("#update").bind("click", function (event) {
         event.preventDefault();
        var i=0;
        var new_record ={};
        $("th").each(function(){
            new_record[$(this).text()] = $("input.up").get(i).value;
            i++;
        });
       
        $("table").append("<tr> </tr>");
        $(".up").each(function () {

            $("tr:last").append("<td>" + $(this).val() + "</td>");
        });
        $(".up").each(function () {

            $(this).val("");

        });
        $("input:first").focus();
        
       $.ajax({
            type:"POST",
            data:encodeURI("newrowdata=" + JSON.stringify(new_record)),
            url:"/new_data",
            async:false,
            dataType:"json",
    });

    });
    userInput.bind("keypress", function (event) {
        if (event.which === 13) {
            $("#update").click();
        }
    });

    
    $("#selectRows").bind("click", function (event) {

        event.preventDefault();
        $("#table td").unbind("click").bind("click", function (event) {
            event.preventDefault();
            var row = $(this).parent();
            row_indices.push(row.index().toString());
            row.children().toggleClass("selectedrow");
        });
        $("#table th").unbind("click").bind("click", function (event) {
            event.preventDefault();
            position = $(this).index();

            $('tr').find('td:eq(' + position + '), th:eq(' + position + ')')
            .toggleClass("selectedcol");
        });
    });


    $("#confirm").click(function (event) {
            event.preventDefault();
        if ($(".selectedrow").length === 0 && $(".selectedcol").length === 0) {
            alert("Please Select Rows or Fields");
        } 
        else {
            if ($(".selectedcol").length !== 0) {
                $("#userInput h5").remove(":contains('" + 
                $("th.selectedcol").text().trim() + "')");
                $("th.selectedcol").each(function(){
                        send.push($(this).text());
                        });
                        
                          $.ajax({
                                    type: "POST",
                                    url:"/del",
                                    data: encodeURI("colname="+ JSON.stringify(send)),
                                    dataType:"json",
                                    aync:false,
                                })
                        $("td.selectedcol,th.selectedcol").remove();
                        }
             else if ($(".selectedrow").length !== 0) {
                $("td.selectedrow").parent().remove();
                  $.ajax 
                  ({
                    type: "GET",
                    url: "/del",
                    data: encodeURI("indices="+ JSON.stringify(row_indices)),
                    dataType: "json",
                    async: false,
                  })
            }
            }
        });
    

    $("#newCol").click(function () {
        var temp, txt;
        var new_cols = [];
        $("#table tr").each(function () {


            if ($(this).is($("tr#headContainer"))) {
                $(this).append("<th class='newfield'> <input class='inpBox' \
                              type='text' value='&nbsp;' ></th>");
            } else {
                $(this).append("<td class='newfield'><input class='inpBox' \
                              type='text' value='&nbsp;'></td>");
            }
        });

        $("td:last-child").children().bind("keypress", function (event) {
            if (event.which === 13) {
                $("#table tr").each(function () {
                    txt = $(this).children().last().children().val();
                    $(this).children().last().text(txt.trim());

                });
                temp = $("tr:first").children().last().text();
                userInput.append("<h5 align = 'left'>&nbsp;Enter " + temp +
                                ":&nbsp;" + "<input  class = 'form-control up' \
                                align='left' type='text' placeholder='" + temp + "'/></h5>");
                 $("td.newfield").each(function(){
                    new_cols.push($(this).text().trim());
                     });

                 $.ajax({
                    type:"GET",
                    data:encodeURI("newcoldata="+ JSON.stringify(new_cols)+"&new_th="+$("th.newfield").text().trim()),
                    dataType:"json",
                    url:"/new_data",
                    async:false,
                    })
                 $("th,td").removeClass("newfield");
            }
        });        
    });

});