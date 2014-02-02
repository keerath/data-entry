/* global document,alert,$ */
$(document).ready(function () {
    "use strict";
    var userInput = $("#userInput"), position;

    $("#table th").each(function () {

        var fieldname = $(this).text().trim();
        userInput.append("<h5 align = 'left'>&nbsp;Enter " + fieldname + ":&nbsp;" 
                        + "<input class = 'form-control up'  align='left' type='text'\
                        placeholder='" + fieldname + "'/></h5>");

    });
    
    $("#info").append("<button type = 'button' title = 'Add Records'\
                     class = 'glyphicon glyphicon-plus-sign btn btn-info btn-sm highlight '\
                     id='update'>&nbsp;Add</buttton> ");
    
    $("#table").on('dblclick', 'td', function (event) {

        event.preventDefault();
        var elem, defaultText;
        defaultText = $(this).text().trim();
        elem = $(this);

        elem.html('<input id= "in" type= "text" value="' + defaultText + '" />');
        elem.children().first().focus();

        $(document).on({
            keypress: function (event) {
                if (event.which === 13) {
                    elem.text(elem.children().val());
                }
            },
            click: function (event) {
                if (event.target.tagName !== "INPUT") {
                    elem.text(elem.children().val());
                }
            }
        });

    });

    $("#update").bind("click", function (event) {

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