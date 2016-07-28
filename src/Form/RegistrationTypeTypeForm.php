<?php

namespace Drupal\registration\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class RegistrationTypeTypeForm.
 *
 * @package Drupal\registration\Form
 */
class RegistrationTypeTypeForm extends EntityForm {

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    $registration_type_type = $this->entity;
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $registration_type_type->label(),
      '#description' => $this->t("Label for the Registration type type."),
      '#required' => TRUE,
    );

    $form['id'] = array(
      '#type' => 'machine_name',
      '#default_value' => $registration_type_type->id(),
      '#machine_name' => array(
        'exists' => '\Drupal\registration\Entity\RegistrationTypeType::load',
      ),
      '#disabled' => !$registration_type_type->isNew(),
    );

    /* You will need additional form elements for your custom properties. */

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $registration_type_type = $this->entity;
    $status = $registration_type_type->save();

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created the %label Registration type type.', [
          '%label' => $registration_type_type->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Registration type type.', [
          '%label' => $registration_type_type->label(),
        ]));
    }
    $form_state->setRedirectUrl($registration_type_type->urlInfo('collection'));
  }

}
