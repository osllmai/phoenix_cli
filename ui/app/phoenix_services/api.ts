// api.ts
import axios from 'axios';

// Define your base URL
const BASE_URL = "http://localhost:18080" || process.env.NEXT_PUBLIC_API_BASE_URL;

// Create an Axios instance
const api = axios.create({
  baseURL: BASE_URL,
  headers: {
    'Content-Type': 'application/json',
  },
});

// Add a request interceptor to include tokens
api.interceptors.request.use(
  (config) => {
    const token = localStorage.getItem('access_token');
    if (token) {
      // Include the token in the Authorization header
      config.headers.Authorization = `Bearer ${token}`; // Adjust if your backend requires 'Bearer ' prefix
    }
    return config;
  },
  (error) => Promise.reject(error)
);

// Add a response interceptor for global error handling
api.interceptors.response.use(
  (response) => response,
  (error) => Promise.reject(error)
);

export default api;
