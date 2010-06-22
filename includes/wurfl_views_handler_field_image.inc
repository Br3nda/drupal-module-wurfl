<?php 

// $Id: wurfl_views_handler_field_image.inc,v 1.1.2.3 2010/06/22 23:47:00 kharbat Exp $

class wurfl_views_handler_field_image extends views_handler_field_markup {
	function construct() {
	   parent::construct();
	}
	
	function render($values) {
		
    if ($values->id) {
    	$file = WURFL_IMAGES . "{$values->id}.gif";
    	if(file_exists($file)) {
    		return theme('image', $file);
    	} else {
    		return theme('image', WURFL_NODEVICE_IMAGE);
    	}
    }
  }
  
	function query() {
	   //$this->add_additional_fields();
	}  
}
