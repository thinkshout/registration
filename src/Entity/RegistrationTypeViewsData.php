<?php

namespace Drupal\registration\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for Registration type entities.
 */
class RegistrationTypeViewsData extends EntityViewsData implements EntityViewsDataInterface {

  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    $data['registration_type']['table']['base'] = array(
      'field' => 'id',
      'title' => $this->t('Registration type'),
      'help' => $this->t('The Registration type ID.'),
    );

    return $data;
  }

}
