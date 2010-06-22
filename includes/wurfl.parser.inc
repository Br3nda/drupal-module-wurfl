<?php 

// $Id: wurfl.parser.inc,v 1.1.2.5 2010/06/22 23:47:00 kharbat Exp $

$wurfl = array();
$curr_event;
$curr_device;
$curr_group;

function wurfl_parse($file) {
	global $wurfl;
	
	if(!file_exists($file)) {
		drupal_set_message('WURFL XML file does not exist in module\'s directory');
		//toLog('parse', $file." does not exist",LOG_ERR);
		return FALSE;
	}

	$xml_parser = xml_parser_create();
	xml_parser_set_option($xml_parser, XML_OPTION_CASE_FOLDING, false);
	xml_set_element_handler($xml_parser, "wurfl_startElement", "wurfl_endElement");
	xml_set_character_data_handler($xml_parser, "wurfl_characterData");

	if (!($fp = fopen($file, "r"))) {
		//toLog('parse', "$file could not be opened for XML input",LOG_ERR);
		die("$file could not open XML input");
	}
	while ($data = fread($fp, 4096)) {
		if (!xml_parse($xml_parser, $data, feof($fp))) {
			$errmsg = sprintf("XML error: %s at line %d",xml_error_string(xml_get_error_code($xml_parser)),xml_get_current_line_number($xml_parser));
			//toLog('parse',$file." ".$errmsg);
			die($file." ".$errmsg);
		}
	}
	
	fclose($fp); 
  xml_parser_free($xml_parser);
  //$devices = &$wurfl["devices"];

	return $wurfl;
}

function wurfl_startElement($parser, $name, $attr) {
	global $wurfl, $curr_event, $curr_device, $curr_group;
	
	switch($name) {
		case "version":
		case "official_url":
		case "ver": 
		case "last_updated":
		case "statement":
			//cdata will take care of these, I'm just defining the array
			$wurfl[$name]="";
			//$curr_event=$wurfl[$name];
			break;
		case "maintainers":
		case "maintainer":
		case "authors":
		case "author":
		case "contributors":
		case "contributor":
			// for the MySQL version I will ignore these (for now)
			// TODO: Add support for non-device WURFL tags
			if ( sizeof($attr) > 0 ) {
				// dirty trick: author is child of authors, contributor is child of contributors
				while ($t = each($attr)) {
					// example: $wurfl["authors"]["author"]["name"]="Andrea Trasatti";
					$wurfl[$name."s"][$name][$attr["name"]][$t[0]]=$t[1];
				}
			}
			break;
		case "device":
			if ( ($attr["user_agent"] == "" || ! $attr["user_agent"]) && $attr["id"]!="generic" ) {
				die("No user agent and I am not generic!! id=".$attr["id"]." HELP");
			}
			if ( sizeof($attr) > 0 ) {
				while ($t = each($attr)) {
					// example: $wurfl["devices"]["ericsson_generic"]["fall_back"]="generic";
					$wurfl["devices"][$attr["id"]][$t[0]]=$t[1];
				}
			}
			$curr_device=$attr["id"];
			break;
		case "group":
			// this HAS NOT to be executed or we will define the id as string and then reuse it as array: ERROR
			//$wurfl["devices"][$curr_device][$attr["id"]]=$attr["id"];
			$curr_group=$attr["id"];
			break;
		case "capability":
			if ( $attr["value"] == 'true' ) {
				$value = true;
			} else if ( $attr["value"] == 'false' ) {
				$value =  false;
			} else {
				$value = $attr["value"];
				$intval = intval($value);
				if ( strcmp($value, $intval) == 0 ) {
					$value = $intval;
				}
			}
			$wurfl["devices"][$curr_device][$curr_group][$attr["name"]]=$value;
			break;
		case "devices":
			// This might look useless but it's good when you want to parse only the devices and skip the rest
			if ( !isset($wurfl["devices"]) )
				$wurfl["devices"]=array();
			break;
		case "wurfl_patch":
			// opening tag of the patch file
		case "wurfl":
			// opening tag of the WURFL, nothing to do
			break;
		case "default":
			// unknown events are not welcome
			die($name." is an unknown event<br />");
			break;
	}	

}

function wurfl_endElement($parser, $name) {
	global $wurfl, $curr_event, $curr_device, $curr_group;
	
	switch ($name) {
		case "group":
			break;
		case "device":
			break;
		case "ver":
		case "last_updated":
		case "official_url":
		case "statement":
			$wurfl[$name]=$curr_event;
			// referring to $GLOBALS to unset curr_event because unset will not destroy 
			// a global variable unless called in this way
			unset($GLOBALS['curr_event']);
			break;
		default:
			break;
	}	
}

function wurfl_characterData($parser, $data) {
	global $curr_event;
	if (trim($data) != "" ) {
		$curr_event.=$data;
		//echo "data=".$data."<br>\n";
	}
}