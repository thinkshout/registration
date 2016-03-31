<?php /**
 * @file
 * Contains \Drupal\registration\Plugin\Field\FieldFormatter\RegistrationForm.
 */

namespace Drupal\registration\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FormatterBase;

/**
 * @FieldFormatter(
 *  id = "registration_form",
 *  label = @Translation("Registration Form"),
 *  field_types = {"registration"}
 * )
 */
class RegistrationForm extends FormatterBase {

  /**
   * @FIXME
   * Move all logic relating to the registration_form formatter into this
   * class. For more information, see:
   *
   * https://www.drupal.org/node/1805846
   * https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Field%21FormatterInterface.php/interface/FormatterInterface/8
   * https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Field%21FormatterBase.php/class/FormatterBase/8
   */

}
