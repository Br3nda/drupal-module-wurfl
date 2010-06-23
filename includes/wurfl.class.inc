<?php

// $Id: wurfl.class.inc,v 1.1.2.5 2010/06/23 18:05:14 kharbat Exp $

// the script will use this to remove useless WAP Gateway chatter
define("REMOVE_REGEX",'/\s?UP\.Link\/[0-9\. ]+/');

class wurfl {
	/**
	 * Internal tracking of the WURFL ID
	 * @var string
	 * @access public
	 */
	var $id="";
	
	/**
	 * Internal tracking of the WURFL User Agent
	 * @var string
	 * @access public
	 */
	var $ua="";	
	var $user_agent ="";	

	/**
	 * If true, Openwave's GUI (mostly wml 1.3) is supported
	 * @var bool
	 * @access public
	 */
	var $GUI=false;

	/**
	 * Device brand (manufacturer)
	 * @var string
	 * @access public
	 */
	var $brand='';

	/**
	 * Device model name
	 * @var string
	 * @access public
	 */
	var $model='';

	/**
	 * If this is a WAP device, this is set to true
	 * @var boolean
	 * @access public
	 */
	var $browser_is_wap=false;

	/**
	 * associative array with all the device's capabilities.
	 * 
	 * Example:  $this->capabilities['downloadfun']['downloadfun_support'] 
	 *	true if downloadfun is supported, otherwise false
	 *
	 * @var associative array
	 * @access public
	 */
	var $capabilities=array();

	/**
	 * HTTP_ACCEPT request headers
	 * Use this to manually set the http-accept string:
	 * 
	 * Example:  $this->http_accept = "text/vnd.wap.wml";
	 * @var string
	 * @access public
	 */
	var $http_accept="";
	
	/**
	 * WURFL ID of the ancestoral device root
	 * This is the ID of the actual device, not a firmware revision
	 *
	 * @var string
	 * @access public
	 */
	var $device_root = '';
	
	/**
	 * Device image path and filename, relative to the class file
	 *
	 * @var string
	 * @access public
	 */
	var $device_image = '';
	
	/**
	 * True if the UA was found in the cache
	 * 
	 * @var boolean
	 * @access public
	 */
	var $found_in_cache = false;
	
	function wurfl($ua = null,  $check_accept=false) {
		$this->http_accept = isset($_SERVER['HTTP_ACCEPT'])? $_SERVER['HTTP_ACCEPT']:'';
		
		if($ua) {
			$this->ua = $ua;
			return $this->getDeviceCapabilities($ua, $check_accept);
		}
	}
	
	function getCapability($capability) {
		if($this->capabilities) {
			$keys = array_keys($this->capabilities);
			$value = false;
			
			foreach($keys as $key) {
				if(is_array($this->capabilities[$key])) {
					$group = $this->capabilities[$key];
					
					if(isset($group[$capability])) {
						$value = $group[$capability];
					}
				}
			}
			
			return $value;
		}
	}
	
	function getDeviceCapabilities($ua, $check_accept=false) {
		$matched = $this->getDeviceCapabilitiesFromAgent($ua, $check_accept);
		return ($matched ? $this->capabilities : false);
	}
	
	function getDeviceCapabilitiesFromAgent($ua, $check_accept=false) {
		// Resetting properties
		$this->device_root = '';
		$this->device_image = '';
		$this->user_agent = $ua;
		$this->found_in_cache = false;
		
		// Search for User Agent in Cache
		$cache = $this->_UserAgentInCache($ua);
		
		if($cache != false) {
			$this->found_in_cache = true;
			$cache_data = $cache->data;
			$this->capabilities = $cache_data['capabilities'];
			$this->_setGlobalProps();			
			
			if($cache_data['device_root'] != ''){
				$this->device_root = $cache_data['device_root'];
			}

			return true;
		}
		
		$ua = trim(preg_replace(REMOVE_REGEX, "", $ua));
		$this->user_agent = $ua;

		if ($check_accept == true) {
			if (!eregi('wml', $this->http_accept) && !eregi('wap', $this->http_accept) && !eregi('xhtml', $this->http_accept)) {
				$this->browser_is_wap=false;
			}else{
				// We can assume this is a mobile device since it accepts wml || wap || xhtml
				$this->browser_is_wap=true;
			}
		}
		
		if(strpos($ua,'/') === false){
			// NO '/' IN USER AGENT??? This is not a WAP device (probably not a real device at all)
			$this->_setGlobalProps();
			return ($this->_getGenericID($ua));
		}

		$device = $this->_UserAgentInDB($this->user_agent);
		
		if(is_array($device)){
			// the exact user agent was in the WURFL - Great!
			$this->_GetFullCapabilities($curr_device['deviceID']);
			$this->capabilities['matched_at'] = $this->user_agent;
			$this->_AddUAToCache($this->user_agent, $this->capabilities, $this->device_root);
			$this->_setGlobalProps();
			return true;
		}

		$ua_len = drupal_strlen($ua);
		
		// The user_agent should not become shorter than 4 characters
		$min_len = 4;		
		$min_len = strpos($ua,'/');
		
		$matched_dev = NULL;
		$niceua = rtrim(drupal_substr($ua, 0, $min_len))."%";
		$minquery = "SELECT id FROM {wurfl_devices} WHERE user_agent LIKE '%s' ORDER BY actual_device_root";
		$result = db_query($minquery, $niceua);
		$num_rows = db_result(db_query("SELECT COUNT(*) FROM {wurfl_devices} WHERE user_agent LIKE '%s' ORDER BY actual_device_root", $niceua));

		if($num_rows == 0) {
			$this->_setGlobalProps();
			return ($this->_getGenericID($ua));
		} else if($num_rows == 1) {
			// Only one match... No use searching
			$matched_dev = db_fetch_object($result)->id;
		  $this->capabilities['matched_at'] = drupal_substr($ua, 0, $min_len);
		  $short_ua = $ua;
		} else {
				// $ua_len should not be longer than the longest agent which
				// matches $niceua to reduce the amount of queries
				unset($result);
				$max_user_agent_len = db_result(db_query("SELECT MAX(CHAR_LENGTH(user_agent)) AS max_user_agent FROM {wurfl_devices} WHERE user_agent LIKE '%s'", $niceua));

				if ($max_user_agent_len < $ua_len) {
					// the longest user agent that we will come across is smaller than
					// this user agent - let's not waste our time here!
					$ua_len = $max_user_agent_len;
				}

				while ($ua_len > $min_len) {
					$short_ua = drupal_substr($ua, 0, $ua_len - 1);
					$niceua = rtrim($short_ua)."%";
					$result = db_query("SELECT id FROM {wurfl_devices} WHERE user_agent LIKE '%s' ORDER BY actual_device_root DESC LIMIT 1", $niceua);
					
					if($matched_dev = db_fetch_object($result)){
						// TODO: add $this->matched_at = $_short_ua
						$matched_dev = $matched_dev->id;
						break;
					}
					// shortening the agent by one each time
					$ua_len--;
				}				
		}
		
		if(!is_null($matched_dev)){
			$this->_GetFullCapabilities($matched_dev);
			$this->_AddUAToCache($this->user_agent,$this->capabilities,$this->device_root);			
			$this->capabilities['matched_at'] = $short_ua;
			$this->_setGlobalProps();
			return true;
		}else{
			// no match was found
			$this->capabilities['matched_at'] = $short_ua;
			$this->_AddUAToCache($this->user_agent, $this->capabilities, $this->device_root);
			$this->_setGlobalProps();
			return false;
		}		
	}
	
	
	/**
	 * This function checks the user agent for signs that it's a WAP device
	 * Returns true if a generic ID is found, otherwise false.
	 * This is a last resort function that is only called if the device in question
	 * does not exist in the WURFL and the class is forced to find another way to
	 * identify the device.
	 *
	 * @param string User agent
	 * @access private
	 * @return boolean success
	 */
	function _getGenericID($ua){
		
		$matched_dev = '';
		$my_is_wireless = false;
		if ( strstr($ua, 'UP.Browser/') && strstr($ua, '(GUI)') ) {
			$matched_dev = 'upgui_generic';
			$my_is_wireless = true;
		} else if ( strstr($ua, 'UP.Browser/') ) {
			$matched_dev = 'uptext_generic';
			$my_is_wireless = true;
		} else if ( eregi('wml', $this->http_accept) || eregi('wap', $this->http_accept) ) {
			$matched_dev = 'generic';
			$my_is_wireless = true;
		} else {
			$matched_dev = 'generic';
			$my_is_wireless = false;
		}
		$this->_GetFullCapabilities($matched_dev);
		$this->capabilities['matched_at']='';
		$this->capabilities['product_info']['is_wireless_device']=$my_is_wireless;
		$this->_setGlobalProps();

		if($my_is_wireless === false) return false;

		return true;
	}
	
	
	function _GetFullCapabilities($id) {

		$_curr_device = $this->_getDeviceCapabilitiesFromId($id);
		// array of full records
		$_capabilities[] = $_curr_device;
		// keep the while loop from running away on an error
		$iteration_limit = 20;
		$i = 0;
		while ( $_curr_device['fall_back'] != 'generic' && $_curr_device['fall_back'] != 'root' && $i <= $iteration_limit) {
			//$this->_toLog('_GetFullCapabilities', 'parent device:'.$_curr_device['fall_back'].' now going to read its capabilities', LOG_INFO);
			$_curr_device = $this->_getDeviceCapabilitiesFromId($_curr_device['fall_back']);
			array_unshift($_capabilities,$_curr_device);
			$i++;
		}
		if($i >= $iteration_limit){
			// the while loop ran away
			return false;
		}
		//$this->_toLog('_GetFullCapabilities', 'getting baseline capabilities from generic device', LOG_INFO);
		$generic = $this->_getDeviceCapabilitiesFromId('generic');
		$_final = $generic;
		// the generic devices are already at the top of the array because I used array_unshift()
		foreach($_capabilities as $curr_device){
			//TODO: Why don't I just array_merge the whole record???? Good question!
			foreach($curr_device as $key => $val) {
				if ( is_array($val) ) {
					$_final[$key] = array_merge($_final[$key], $val);
				} else {
					$_final[$key] = $val;
				}
			}
		}
		$this->capabilities = $_final;
		
		return true;
	}	
		
	/**
	 * Given a device id reads its capabilities
	 *
	 * @param string device's wurfl_id
	 * @access private
	 * @return mixed boolean false if not identified or array capabilities
	 *
	 */	
	function _getDeviceCapabilitiesFromId($id) {
		if(count($this->errors) != 0) return(false);
		
		if ($id == 'upgui_generic' ) {
			$this->GUI = true;
		}
		
		$result = db_query("SELECT * FROM {wurfl_devices} WHERE id = '%s'", $id);
		$num_rows = db_result(db_query("SELECT COUNT(*) FROM {wurfl_devices} WHERE id = '%s'", $id));
		
		if($num_rows > 0){
			$device = db_fetch_array($result);
			if($this->device_root == '' && $device['actual_device_root'] == 1){
				$this->device_root = $device['deviceID'];
				//$image = IMAGE_DIR.$device['deviceID'].".gif";
				// PHP evaluates from left to right, so "file_exists" will not get
				// called if IMAGE_CHECKING is false
				/*if(IMAGE_CHECKING && file_exists($image)){
					$this->device_image = $image;
					$this->_toLog("_getDeviceCapabilitiesFromId","device image found: $image",LOG_INFO);
				}*/
			}
			$cap = unserialize($device['capabilities']);
			return $cap;
		}else{
			// device is not in the WURFL
			// deal with it appropriately
			return false;
		}		
	}
	
	/**
	 * Checks to see if a given user agent is in the WURFL database
	 * 
	 * @param string user agent
	 * @return mixed false if not in DB, else full device record array
	 * @access private
	 */
	function _UserAgentInDB($ua){
		//TODO: using LIKE will do the search case-insensitive, but = is much faster
		$query = db_query("SELECT * FROM {wurfl_devices} WHERE LOWER(user_agent) = '%s'", drupal_strtolower($ua));
		$device = db_fetch_object($query);
		return ($device ? $device : false);
	}
	
	/**
	 * Checks to see if a given user agent is in the cache
	 * 
	 * @param string user agent
	 * @return mixed false if not in cache, else full device cache data array
	 * @access private
	 */
	function _UserAgentInCache($ua){
		$cache = cache_get($ua);
		return ($cache ? $cache : false);
	}
	
	/**
	 * Adds a user agent and its associated data to the cache
	 * 
	 * @param string user agent
	 * @param array capabilities
	 * @param string device root
	 * @return mixed false if not in cache, else full device cache data array
	 * @access private
	 */
	function _AddUAToCache($ua, $cap, $devroot){
		$ua = trim(preg_replace(REMOVE_REGEX, "", $ua));
		
		if(strlen($ua) < 3)
			return true;
		
		$data = array("capabilities" => $cap, "device_root" => $devroot);
		return cache_set($ua, $data);
	}

	function _setGlobalProps(){
		$this->GUI = false;
		if(!isset($this->capabilities['product_info']['is_wireless_device'])){
			$this->capabilities['product_info'] = array();
			// set this to false by default
			$this->capabilities['product_info']['is_wireless_device'] = false;
			$this->capabilities['product_info']['brand_name'] = '';
			$this->capabilities['product_info']['model_name'] = '';
		}
		unset($this->wurfl_agent);
		unset($this->browser_is_wap);
		unset($this->brand);
		unset($this->model);
		unset($this->id);
		
		$this->wurfl_agent = $this->capabilities['user_agent'];
		$this->browser_is_wap = $this->capabilities['product_info']['is_wireless_device'];
		$this->brand = $this->capabilities['product_info']['brand_name'];
		$this->model = $this->capabilities['product_info']['model_name'];
		$this->id = $this->capabilities['id'];
		
		$image = WURFL_IMAGES . "{$this->id}.gif";
		$this->device_image = (file_exists($image) ? $image : WURFL_NODEVICE_IMAGE);
	}
}