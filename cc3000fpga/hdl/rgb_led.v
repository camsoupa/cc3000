
module rgb_led
(
	pclk,
	nreset,
	bus_write_en, 
	bus_read_en,
	bus_addr,
	bus_write_data,
	bus_read_data,
    pwm_r, pwm_g, pwm_b
);


`define HZ_PER_COLOR_DIVISION 16'd3921

// 100Hz pwm
`define PWM_PERIOD 32'd1000000

// bus lines	
input pclk, nreset, bus_write_en, bus_read_en;
input [7:0] bus_addr;
input [31:0] bus_write_data;
output reg [31:0] bus_read_data;
output reg pwm_r, pwm_g, pwm_b;

// counts to PWM_PERIOD
reg [31:0] duty_cycle_counter;

reg [31:0] brightness;
reg [31:0] brightness_factor;
reg [7:0]  delta_brightness;

// led state registers
// the led toggles between state 1 and state 2

reg [31:0] led_control;

// 0x En (Enabled bit) P (pulse_rate) RR GG BB
reg [31:0] pending_led_state_1;
reg [31:0] pending_led_state_2;

reg [31:0] led_state_1;
reg [31:0] led_state_2;

// which led state are we in?
reg [31:0] current_state;

reg enabled;
reg pulse;
reg red;
reg green;
reg blue;

always@(posedge pclk)
if(~nreset)
  begin
	pending_led_state_1 <= 32'h00000000;
    pending_led_state_2 <= 32'h00000000;
  end
else begin
	if(bus_write_en) begin : WRITE
		case(bus_addr[3:2])
			2'b00: // led_control
                begin 
				led_control <= bus_write_data;
                end
            2'b01: // pending_led_state_1
                begin
                pending_led_state_1 <= bus_write_data;
                end
            2'b10: // pending_led_state_2
                begin 
                pending_led_state_2 <= bus_write_data;
                end
        endcase
    end
	else if(bus_read_en) begin : READ
        case(bus_addr[3:2])
	        2'b00: // led_control
                begin 
		        bus_read_data <= led_control;
				end
            2'b01: // pending_led_state_1
                begin 
                bus_read_data <= pending_led_state_1;
				end
            2'b10: // pending_led_state_2
                begin 
                bus_read_data <= pending_led_state_2;
			    end
        endcase
     end
end

always@(posedge pclk) begin
    enabled <= current_state[31];
    pulse   <= current_state[30:24];
    red 	<= current_state[23:16];
    green 	<= current_state[15:8];
    blue	<= current_state[7:0];
end

// changing global brightness
always@(posedge pclk) begin
	if(brightness == 32'd100000000) begin
    	delta_brightness = -1;
    end
    else begin
		if(brightness == 32'd0) begin
			delta_brightness = 1;
		end
		else begin
			brightness = brightness + delta_brightness;
		end
    end
	brightness_factor <= ((`HZ_PER_COLOR_DIVISION * brightness) / 32'd100000000);
end

// pulsing colors based on global brightness
always@(posedge pclk) begin
if(~nreset)begin
    led_state_1 <= 32'h00000000;
    led_state_2 <= 32'h00000000;
	pwm_r <= 0;
    pwm_g <= 0;
    pwm_b <= 0;
end
else begin
	duty_cycle_counter <= duty_cycle_counter + 1;
	if(duty_cycle_counter == `PWM_PERIOD) begin
    	duty_cycle_counter <= 0;
    end
    else begin
       //turn off signals that have reached their duty cycle
		if(red * brightness_factor == duty_cycle_counter) begin
			pwm_r <= 0;
		end
		if(blue * brightness_factor == duty_cycle_counter) begin
			pwm_b <= 0;
		end
		if(green * brightness_factor == duty_cycle_counter) begin
			pwm_g <= 0;
		end
    end

	// "overflow" of the pwm counter, so start a new duty cycle
    if(duty_cycle_counter == 0)begin
		// toggle state
		if(current_state == led_state_1)begin
			current_state <= led_state_2;
		end
		else begin
			current_state <= led_state_1;
		end

		led_state_1 <= pending_led_state_1;
		led_state_2 <= pending_led_state_2;

    	//turn on all pwm signals
		//TODO only if they are non-zero values
		pwm_r <= 1;
        pwm_g <= 1;
        pwm_b <= 1;
    end
end
end

endmodule