
let machine_running = false;

$(document).ready( function(){
    const refreshApp = setInterval(updateValues, 1000);
});

function updateValues()
{
    let datestring = new Date()
    $('#currentdatetime').html(datestring.toString().substring(0,24));

    $.get("/getstatus", function(data){
        console.log(data);
        const status = JSON.parse(data);
        $('#temp-value').html(status.temperature);
        $('#moisture-value').html(status.moisture);
        $('#feed-value').html(status.feeding);
        $('#time-value').html(status.uptime);


        if(status.running == 1){
            machine_running = true;
        }
        else{
            machine_running = false;
        }

    });

    $('#run-btn').prop("checked", machine_running);
    console.log($('#run-btn').prop('checked'));
    
}

function toggleMachine()
{
    alert("yoo")
}
