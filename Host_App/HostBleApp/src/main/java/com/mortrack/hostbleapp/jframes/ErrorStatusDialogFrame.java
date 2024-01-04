// TODO: Use the Interface Class part used in the Factory Design Pattern to make @ref CALLING_CLASS Global Variable to universally adapt to several other JFrames.
/**@file
 * 
 * @defgroup Error_Status_Dialog_Frame Error Status Dialog Frame module
 * @{
 * 
 * @brief   This module contains the methods and event handlers required to
 *          generate and show to the user an Error Status Dialog Frame as part
 *          of the UI so that this UI can show Error Messages to the user.
 * 
 * @details The following gives a friendly code example of how to use
 *          {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame}
 *          Class, which is easily achieved with a single code of line:
 * @code
   new ErrorStatusDialogFrame("", "The given Payload File Path does not exist.", this); // Call this from wherever you want from the MainFrame Class.
 * @endcode
 * 
 * @details Another friendly code example is the following:
 * @code
   int codeNumber = 17;
   new ErrorStatusDialogFrame("ETX OTA Exception Code = " + codeNumber, "The requested Payload could not be sent to the slave device.", this);  // Call this from wherever you want from the MainFrame Class.
 * @endcode
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    November 08, 2023.
 */


/* @brief   Package containing all the classes, methods, variables and code in
 *          general to generate the Frames of the Host BLE App UI.
 *
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 30, 2023.
 */
package com.mortrack.hostbleapp.jframes;

import java.awt.Color;
import java.awt.Font;
import javax.swing.BorderFactory;
import javax.swing.SwingConstants;

/**@brief   Main Class containing all the methods required to generate, make
 *          visible and to handle the events of the Error Status Dialog Frame
 *          of UI.
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    November 08, 2023.
 */
public class ErrorStatusDialogFrame extends javax.swing.JFrame {
    /**@brief   Color Object containing the RBG color designated for the OK
     *          Button in it is not being clicked with the left button of the
     *          mouse or, in other words, whenever it is in its normal state.
     */
    private static final Color OK_BUTTON_BG_NO_CLICK_COLOR = new Color(255,255,255);
    
    /**@brief   Color Object containing the RBG color designated for the OK
     *          Button in it is being clicked state with the left button of the
     *          mouse.
     */
    private static final Color OK_BUTTON_BG_CLICK_COLOR = new Color(79,0,0);
    
    /**@brief   Color Object containing the RBG color designated for the Border
     *          of the OK Button.
     */
    private static final Color OK_BUTTON_BORDER_COLOR = new Color(184,0,0);
    
    /**@brief   Border Width designated for the OK Button for whenever it is not
     *          being hovered by the mouse cursor.
     */
    private static final int OK_BUTTON_NO_HOVER_BORDER_WIDTH = 2;
    
    /**@brief   Border Width designated for the OK Button for whenever it is
     *          being hovered by the mouse cursor.
     */
    private static final int OK_BUTTON_HOVER_BORDER_WIDTH = 4;
    
    /**@brief   Font Size designated for the OK Button. */
    private static final float OK_BUTTON_FONT_SIZE = 18;
    
    /**@brief   Message to be assigned to the row 1 (i.e., the 
     *          {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame#jLabelMessageRow1})
     *          of the Body of the Error Status Dialog Frame.
     */
    private final String MESSAGE1;
    
    /**@brief   Message to be assigned to the row 2 (i.e., the 
     *          {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame#jLabelMessageRow2})
     *          of the Body of the Error Status Dialog Frame.
     */
    private final String MESSAGE2;
    
    /**@brief   JFrame Class from which the Error Status Dialog Frame was called
     *          from.
     */
    private final MainFrame CALLING_CLASS;
            
    /**@brief   Constructor of the
     *          {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame}
     *          Class, which persists the arguments given, disables the JFrame
     *          Class from which this Constructor was called from, initializes
     *          the UI components of the
     *          {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame}
     *          Class.
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public ErrorStatusDialogFrame(String message1, String message2, MainFrame callingClass) {
        this.MESSAGE1 = message1;
        this.MESSAGE2 = message2;
        this.CALLING_CLASS = callingClass;
        this.CALLING_CLASS.setEnabled(false);
        initComponents();
        jLabelMessageRow1.setText(this.MESSAGE1);
        jLabelMessageRow2.setText(this.MESSAGE2);
        jLabelMessageRow1.setHorizontalAlignment(SwingConstants.CENTER);
        jLabelMessageRow2.setHorizontalAlignment(SwingConstants.CENTER);
        this.setVisible(true); // This has to be at the end of the Constructor Class.
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPanel2 = new javax.swing.JPanel();
        jLabelCheckIcon = new javax.swing.JLabel();
        jLabelSuccessMessage = new javax.swing.JLabel();
        jPanelOkButton = new javax.swing.JPanel();
        jLabelButtonOk = new javax.swing.JLabel();
        jLabelMessageRow1 = new javax.swing.JLabel();
        jLabelMessageRow2 = new javax.swing.JLabel();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setBackground(new java.awt.Color(255, 255, 255));
        setUndecorated(true);

        jPanel2.setBackground(new java.awt.Color(184, 0, 0));

        jLabelCheckIcon.setIcon(new javax.swing.ImageIcon("C:\\Users\\cmira\\OneDrive\\Escritorio\\Bootloader Project v0.4\\Host_BLE_App\\HostBleApp\\img\\StatusStatusDialog_CrossIcon.png")); // NOI18N

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(jLabelCheckIcon)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addContainerGap(21, Short.MAX_VALUE)
                .addComponent(jLabelCheckIcon)
                .addContainerGap(15, Short.MAX_VALUE))
        );

        jLabelSuccessMessage.setFont(new java.awt.Font("Arial", 1, 26)); // NOI18N
        jLabelSuccessMessage.setText("Error!");

        jPanelOkButton.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(184, 0, 0), 2));
        jPanelOkButton.setToolTipText("");
        jPanelOkButton.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jPanelOkButtonMouseClicked(evt);
            }
            public void mouseEntered(java.awt.event.MouseEvent evt) {
                jPanelOkButtonMouseEntered(evt);
            }
            public void mouseExited(java.awt.event.MouseEvent evt) {
                jPanelOkButtonMouseExited(evt);
            }
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jPanelOkButtonMousePressed(evt);
            }
        });

        jLabelButtonOk.setFont(new java.awt.Font("Arial", 0, 18)); // NOI18N
        jLabelButtonOk.setForeground(new java.awt.Color(184, 0, 0));
        jLabelButtonOk.setText("OK");

        javax.swing.GroupLayout jPanelOkButtonLayout = new javax.swing.GroupLayout(jPanelOkButton);
        jPanelOkButton.setLayout(jPanelOkButtonLayout);
        jPanelOkButtonLayout.setHorizontalGroup(
            jPanelOkButtonLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanelOkButtonLayout.createSequentialGroup()
                .addContainerGap(52, Short.MAX_VALUE)
                .addComponent(jLabelButtonOk)
                .addContainerGap(51, Short.MAX_VALUE))
        );
        jPanelOkButtonLayout.setVerticalGroup(
            jPanelOkButtonLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanelOkButtonLayout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(jLabelButtonOk)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jLabelMessageRow1.setFont(new java.awt.Font("Arial", 0, 16)); // NOI18N
        jLabelMessageRow1.setForeground(new java.awt.Color(77, 77, 77));
        jLabelMessageRow1.setText("Message mesage message message message");

        jLabelMessageRow2.setFont(new java.awt.Font("Arial", 0, 16)); // NOI18N
        jLabelMessageRow2.setForeground(new java.awt.Color(77, 77, 77));
        jLabelMessageRow2.setText("Message mesage message message message");
        jLabelMessageRow2.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addGap(0, 0, Short.MAX_VALUE)
                .addComponent(jPanelOkButton, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
            .addGroup(layout.createSequentialGroup()
                .addContainerGap(30, Short.MAX_VALUE)
                .addComponent(jLabelMessageRow2, javax.swing.GroupLayout.PREFERRED_SIZE, 484, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(30, Short.MAX_VALUE))
            .addGroup(layout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(jLabelSuccessMessage)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
            .addGroup(layout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(jLabelMessageRow1, javax.swing.GroupLayout.PREFERRED_SIZE, 484, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addComponent(jPanel2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(35, 35, 35)
                .addComponent(jLabelSuccessMessage)
                .addGap(18, 18, 18)
                .addComponent(jLabelMessageRow1)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jLabelMessageRow2)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 30, Short.MAX_VALUE)
                .addComponent(jPanelOkButton, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(20, 20, 20))
        );

        pack();
        setLocationRelativeTo(null);
    }// </editor-fold>//GEN-END:initComponents

    /**@brief   Event Handler method for whenever the mouse cursor hovers the
     *          {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame#jPanelOkButton}
     *          JPanel that acts as the OK Button.
     * 
     * @details During the period of time at which this Event Handler method is
     *          called, a highlight effect on the OK Button will be made by
     *          enhancing its Border Width and by changing its OK Text from
     *          Plain to Bold.
     * 
     * @param evt   Mouse Entered Event Handler Object for the
     *              {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame#jPanelOkButton}
     *              JPanel that acts as the OK Button.
     *
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    private void jPanelOkButtonMouseEntered(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jPanelOkButtonMouseEntered
        jPanelOkButton.setBackground(OK_BUTTON_BG_NO_CLICK_COLOR);
        jPanelOkButton.setBorder(BorderFactory.createLineBorder(OK_BUTTON_BORDER_COLOR, OK_BUTTON_HOVER_BORDER_WIDTH));
        jLabelButtonOk.setFont(jLabelButtonOk.getFont().deriveFont(Font.BOLD, OK_BUTTON_FONT_SIZE));
    }//GEN-LAST:event_jPanelOkButtonMouseEntered

    /**@brief   Event Handler method for whenever the mouse cursor does not
     *          hover the {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame#jPanelOkButton}
     *          JPanel that acts as the OK Button.
     * 
     * @details During the period of time at which this Event Handler method is
     *          called, the OK Button will be shown with its normal Border Width
     *          and with its OK Text in Plain Type.
     * 
     * @param evt   Mouse Exited Event Handler Object for the
     *              {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame#jPanelOkButton}
     *              JPanel that acts as the OK Button.
     *
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    private void jPanelOkButtonMouseExited(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jPanelOkButtonMouseExited
        jPanelOkButton.setBackground(OK_BUTTON_BG_NO_CLICK_COLOR);
        jPanelOkButton.setBorder(BorderFactory.createLineBorder(OK_BUTTON_BORDER_COLOR, OK_BUTTON_NO_HOVER_BORDER_WIDTH));
        jLabelButtonOk.setFont(jLabelButtonOk.getFont().deriveFont(Font.PLAIN, OK_BUTTON_FONT_SIZE));
    }//GEN-LAST:event_jPanelOkButtonMouseExited

    /**@brief   Event Handler method for whenever there is a mouse cursor click
     *          on the {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame#jPanelOkButton}
     *          JPanel that acts as the OK Button.
     * 
     * @details If this Event Handler method is called, then the
     *          {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame#CALLING_CLASS}
     *          will be enabled again and the
     *          current Object from this
     *          {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame}
     *          Class will be terminated. Therefore, the Success Status Dialog
     *          Frame will be closed.
     * 
     * @param evt   Mouse Clicked Event Handler Object for the
     *              {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame#jPanelOkButton}
     *              JPanel that acts as the OK Button.
     *
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    private void jPanelOkButtonMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jPanelOkButtonMouseClicked
        this.CALLING_CLASS.setEnabled(true);
        super.dispose();
    }//GEN-LAST:event_jPanelOkButtonMouseClicked

    /**@brief   Event Handler method for whenever there is a mouse cursor press
     *          on the {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame#jPanelOkButton}
     *          JPanel that acts as the OK Button.
     * 
     * @details During the period of time at which this Event Handler method is
     *          called, a push-button-effect on the OK Button will be made by
     *          changing its background color to a darker one.
     * 
     * @param evt   Mouse Pressed Event Handler Object for the
     *              {@link com.mortrack.hostbleapp.jframes.ErrorStatusDialogFrame#jPanelOkButton}
     *              JPanel that acts as the OK Button.
     *
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    private void jPanelOkButtonMousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jPanelOkButtonMousePressed
        jPanelOkButton.setBackground(OK_BUTTON_BG_CLICK_COLOR);
        jPanelOkButton.setBorder(BorderFactory.createLineBorder(OK_BUTTON_BORDER_COLOR, OK_BUTTON_NO_HOVER_BORDER_WIDTH));
        jLabelButtonOk.setFont(jLabelButtonOk.getFont().deriveFont(Font.PLAIN, OK_BUTTON_FONT_SIZE));
    }//GEN-LAST:event_jPanelOkButtonMousePressed
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel jLabelButtonOk;
    private javax.swing.JLabel jLabelCheckIcon;
    private javax.swing.JLabel jLabelMessageRow1;
    private javax.swing.JLabel jLabelMessageRow2;
    private javax.swing.JLabel jLabelSuccessMessage;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanelOkButton;
    // End of variables declaration//GEN-END:variables
}
