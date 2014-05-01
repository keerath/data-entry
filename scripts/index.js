$(document).ready(function(){
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
							alert(response);
						}
					}		
				
				});		
	});
});