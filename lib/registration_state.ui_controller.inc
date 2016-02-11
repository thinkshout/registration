<?php

/**
 * @file
 * UI Controller for Registration states.
 */
// TODO: Replace DEfaultUIController
//class RegistrationStatesUIController extends EntityDefaultUIController {
class RegistrationStatesUIController {
  public function overviewForm($form, &$form_state) {
    return drupal_get_form('registration_states_overview_form');
  }

}
