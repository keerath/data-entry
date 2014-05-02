$(document).ready(function(){
	$("#connAlert").hide();
	$("#conn").click(function(event){
		event.preventDefault();
		var data = $("#init").serialize();
		$.ajax
				({
					data:data,
					type:"POST",
					async:false,
					url:"/init",
					dataType:"text",
					success:function(response)
					{
						if(response === "Connection Successful")
						{
							window.open("table.html","_self");
						}
						else
						{
							$("#alert-content").append("<strong>Connection Error</strong>&nbsp;"+response);
							$("#connAlert").show();
						}
					}		
				
				});		
	});
	$("button.close").click(function(event){
		event.preventDefault();
		$("#connAlert").hide();
		$("input").val('');
		$("input").first().focus();
		$("#alert-content").html("");
	});
});