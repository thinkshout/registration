<?php

/**
 * @file
 * The class used for model type entities
 */
class RegistrationType extends Entity {

  public $name;
  public $label;
  public $locked;
  public $default_state;
  public $data;
  public $registrant_entity_type;
  public $registrant_bundle;
  public $registrant_email_property;

  public function __construct($values = array()) {
    parent::__construct($values, 'registration_type');
  }
}
