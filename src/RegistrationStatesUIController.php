<?php
namespace Drupal\registration;

/**
 * @file
 * UI Controller for Registration states.
 */
class RegistrationStatesUIController extends EntityDefaultUIController {

  public function overviewForm($form, &$form_state) {
    return \Drupal::formBuilder()->getForm('registration_states_overview_form');
  }

}
