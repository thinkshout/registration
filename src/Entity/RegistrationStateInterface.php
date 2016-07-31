<?php

namespace Drupal\registration\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Registration state entities.
 *
 * @ingroup registration
 */
interface RegistrationStateInterface extends ContentEntityInterface, EntityChangedInterface, EntityOwnerInterface {

  // Add get/set methods for your configuration properties here.

  /**
   * Gets the Registration state name.
   *
   * @return string
   *   Name of the Registration state.
   */
  public function getName();

  /**
   * Sets the Registration state name.
   *
   * @param string $name
   *   The Registration state name.
   *
   * @return \Drupal\registration\Entity\RegistrationStateInterface
   *   The called Registration state entity.
   */
  public function setName($name);

  /**
   * Gets the Registration state creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Registration state.
   */
  public function getCreatedTime();

  /**
   * Sets the Registration state creation timestamp.
   *
   * @param int $timestamp
   *   The Registration state creation timestamp.
   *
   * @return \Drupal\registration\Entity\RegistrationStateInterface
   *   The called Registration state entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns the Registration state published status indicator.
   *
   * Unpublished Registration state are only visible to restricted users.
   *
   * @return bool
   *   TRUE if the Registration state is published.
   */
  public function isPublished();

  /**
   * Sets the published status of a Registration state.
   *
   * @param bool $published
   *   TRUE to set this Registration state to published, FALSE to set it to unpublished.
   *
   * @return \Drupal\registration\Entity\RegistrationStateInterface
   *   The called Registration state entity.
   */
  public function setPublished($published);

}
