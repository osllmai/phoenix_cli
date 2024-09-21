// auth.ts
import api from './api';

export interface UserWorkspace {
  created_at: string;
  default_context_length: number;
  default_model: string;
  default_prompt: string;
  default_temperature: number;
  description: string;
  embeddings_provider: string;
  id: string;
  include_profile_context: boolean;
  include_workspace_instructions: boolean;
  instructions: string;
  is_home: boolean;
  name: string;
  updated_at: string;
  user_id: string;
}

interface LoginResponse {
  access_token: string;
  refresh_token: string;
  workspace: UserWorkspace
}


export const login = async (email: string, password: string): Promise<LoginResponse> => {
  const response = await api.post<LoginResponse>('/login', {email, password});
  return response.data;
};

interface RegisterResponse {
  // Define the response fields based on your API
}

export const register = async (
  email: string,
  password: string,
  otherParams?: Record<string, any>
): Promise<RegisterResponse> => {
  const response = await api.post<RegisterResponse>('/register', {email, password, ...otherParams});
  return response.data;
};
