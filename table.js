/* global document,alert,$ */
$(document).ready(function () {
    "use strict";
    var userInput = $("#userInput"), position;
    $("#table").on('dblclick', 'td', function (event) {
        var flag =1;
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
         $.ajax
          ({
            type:"POST",
            async:false,
            url:"/update_data",
            data: encodeURI("celldata="+elem.text()),s
            dataType:"text",
          })

        }
        

    });

    $("#update").bind("click", function (event) {
        var data = $("#userInput").serialize();
        $.ajax({
            type:"POST",
            data:data,
    });
        event.preventDefault();
        $("table").append("<tr> </tr>");
        $(".up").each(function () {

            $("tr:last").append("<td>" + $(this).val() + "</td>");
        });
        $(".up").each(function () {

            $(this).val("");

        });
        $("input:first").focus();
      

    });
    userInput.bind("keypress", function (event) {
        if (event.which === 13) {
            $("#update").click();
        }
    });

    
    $("#selectRows").bind("click", function () {
        $("#table td").unbind("click").bind("click", function () {
            var row = $(this).parent();
            row.children().toggleClass("selectedrow");
        });
        $("#table th").unbind("click").bind("click", function () {
            position = $(this).index();

            $('tr').find('td:eq(' + position + '), th:eq(' + position + ')')
            .toggleClass("selectedcol");
        });
    });


    $("#confirm").click(function () {
        if ($(".selectedrow").length === 0 && $(".selectedcol").length === 0) {
            alert("Please Select Rows or Fields");
        } else {
            if ($(".selectedcol").length !== 0) {
                $("#userInput h5").remove(":contains('" + 
                $("th.selectedcol").text().trim() + "')");
                $("td.selectedcol,th.selectedcol").remove();
            } else if ($(".selectedrow").length !== 0) {
                $("td.selectedrow").parent().remove();
            }
        }
    });

    $("#newCol").click(function () {
        var temp, txt;
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
                    $(this).children().last().text(txt);

                });
                temp = $("tr:first").children().last().text();
                userInput.append("<h5 align = 'left'>&nbsp;Enter " + temp +
                                ":&nbsp;" + "<input  class = 'form-control up' \
                                align='left' type='text' placeholder='" + temp + "'/></h5>");
            }

        });
    });

});