#! /usr/bin/python3.4
# -*- coding: utf-8 -*-
__author__ = 'Ryan A. Rodriguez'


# from samiio import ApiClient
# from MessagesApi import MessagesApi
# from models import *
#
# apiClient = ApiClient(apiKey=deviceToken, apiServer="https://api.samsungsami.io/v1.1")
# messagesApi = MessagesApi(apiClient)
# # Get the latest message:
# lastNormalizedMessagesEnvelope = messagesApi.getNormalizedMessagesLast(sdids=deviceID, count=1)
# print 'Got the last normalized message:' + str(lastNormalizedMessagesEnvelope.data[0].data)
#
#
# # Define a device with token, id
# class Device():
#     def __init__():
#         pass
#
#
# class SamiMessenger():
#     # Do we want this to work for many device id's? I.e. instead of a single device (Artik) communicating with db,
#     # each of the sensors gets assigned an ID and the Artik sends the messages on their behalf. If the former, we'll
#     # maybe need to keep track of devices in a dict, keep an ApiClient object for each.
#     # devices = {deviceID: ApiClient(deviceToken, ...), ...}
#     deviceID = "YOUR DEVICE ID"
#     deviceToken = "YOUR DEVICE TOKEN"
#     apiClient = ApiClient(apiKey=deviceToken, apiServer="https://api.samsungsami.io/v1.1")
#
#     def __init__():
#         self.messagesApi = MessagesApi(self.apiClient)
#
#     def send_message(self, data, deviceID):
#         message = Message.Message()
#         message.sdid = deviceID
#         message.type = "message"
#         # message.data = {'numberOfSteps':2000, 'distance': 1.1, 'floorsAscended': 2, 'floorsDescended': 10}
#         message.data = data
#         messageIDEnvelope = messagesApi.postMessage(body=message)
#
#     # The message ID -> messageIDEnvelope.data.mid
#
#     def get_message(self, data, deviceID):
#         messageIDEnvelope = messagesApi.postMessage(body=message)
