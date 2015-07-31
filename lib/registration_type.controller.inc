<?php

/**
 * @file
 * The controller class used for registration type entities
 */
class RegistrationTypeController extends EntityAPIControllerExportable {

  public function save($entity, DatabaseTransaction $transaction = NULL) {
    // Move the data properties into data before saving.
    $data_properties = array('held_expire', 'held_expire_state');
    foreach ($data_properties as $data_property) {
      $entity->data[$data_property] = $entity->$data_property;
    }

    parent::save($entity, $transaction);
  }

}
