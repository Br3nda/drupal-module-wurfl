<?php 

// $Id: wurfl_views_handler_field_image.inc,v 1.1.2.4 2010/06/26 23:43:28 kharbat Exp $

class wurfl_views_handler_field_image extends views_handler_field_markup {
	function construct() {
	   parent::construct();
	}
	
	function render($values) {
    if ($values->wurfl_devices_id) {
    	$file = WURFL_IMAGES . "{$values->wurfl_devices_id}.gif";
    	if(file_exists($file)) {
    		return theme('image', $file);
    	}
    }
  	
  	return theme('image', WURFL_NODEVICE_IMAGE);
	}
  
	function query() {
	   //$this->add_additional_fields();
	}  
}
